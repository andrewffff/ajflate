
#ifndef _BSD_SOURCE
#define _BSD_SOURCE // make dirent nicer on linux
#endif

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <arpa/inet.h>

using namespace std;
typedef unsigned char uchar;

#include "../contrib/crc32.h"

#include "intarray.h"

FILE* g_log; // may be NULL

//
// Hash functions used by various compressors
//
class Hasher {
protected:
	unsigned int _mod;

public:
	// return a hash of the data at *p - up to four bytes may be
	// used and must be available
	virtual int operator()(const uchar* p) const = 0;

	virtual ~Hasher() { return; }
};

class HashZlib : public Hasher {
public:
	HashZlib(int mod = 0)  { _mod = mod ? mod : 32768; }

	int operator()(const uchar* p) const {
		return ((p[0]<<10) ^ (p[1]<<5) ^ (p[2])) % _mod;
	}
};


class HashPutty : public Hasher {
public:
	HashPutty(int mod = 0) { _mod = mod ? mod : 2039; }

	int operator()(const uchar* p) const {
		return (257*p[0] + 263*p[1] + 269*p[2]) % _mod;
	}
};

class HashLZJB : public Hasher {
public:
	HashLZJB(int mod = 0) { _mod = mod ? mod : 256; }

	int operator()(const uchar* p) const {
		return ((p[0] + 13) ^ (p[1] - 13) ^ p[2]) % _mod;
	}
};

class HashLZRW1 : public Hasher {
public:
	HashLZRW1(int mod = 0) { _mod = mod ? mod : 4096; }

	int operator()(const uchar* p) const {
		return ((((p[0]<<8) ^ (p[1]<<4) ^ p[2]) * 40543) >> 4) % _mod;
	}
};

class HashLZO1X : public Hasher {
public:
	HashLZO1X(int mod = 0) { _mod = mod ? mod : 16384; }

	int operator()(const uchar* p) const {
		return ((((p[0]<<16) ^ (p[1]<<10) ^ (p[2]<<5) ^ p[3]) * 33) >> 5) % _mod;
	}
};

class HashAF1 : public Hasher {
public:
	HashAF1(int mod = 0) { _mod = mod ? mod : 256; }

	int operator()(const uchar* p) const {
		return (19*p[0] + 13*p[1] + 7*p[2] + p[3]) % _mod;
	}
};

// Our other core primitive is the dictionary, which given a hash
// function implements a hash table (with posssible secondary lookup
// etc)
enum DictionaryEntryType {
	MATCH_NONE = 88,
	MATCH_START = 97,
	MATCH_WITHIN = 101
};

class Dictionary {
protected:
	const char* _name;

public:
	virtual ~Dictionary() { return; }

	const char* name() const {
		return _name ? _name : "(anon)";
	}

	// Returns a complete list of all the potential matches for
	// the current text (starts at data[ofs]). At least four input
	// bytes are available
	virtual vector<int> potentialMatches(const uchar* data, int ofs) const = 0;

	// Insert a potential match into our lookup stuff. Data starts
	// at data[ofs] and at least four bytes must be available. We should
	// be told whether the potential match is:
	//    * MATCH_NONE - not part of a match
	//    * MATCH_START - the first character within a match
	//    * MATCH_WITHIN - within a match, not the first character
	virtual void insert(const uchar* data, int ofs, DictionaryEntryType t) = 0;
};

class NullDictionary : public Dictionary {
public:
	NullDictionary() { _name = "null"; }
	vector<int> potentialMatches(const uchar*, int) const { return vector<int>(); }
	void insert(const uchar*, int, DictionaryEntryType) { return; }
};


#include "linkedlistchain.h"
#include "doublehashtable.h"

// Construct a set of "standard" dictionary/hash function combinations
//     chainStrategy can be:
//       - one of the ChainStrategy enums
//       - a maximum chain length (1 for a simple 1-way associative hash table)
//       - 0 to use the hash algorithm's preferred chain length

vector<Dictionary*> makeDictionaries(int hashTableSize = 0, int chainStrategy = 0, bool *allowInMatch = 0) {
	vector<Dictionary*> v;

	if(chainStrategy >= 0) {
		v.push_back(new LinkedListChain(new HashZlib(hashTableSize),  "zlib-1",  chainStrategy>0 ? chainStrategy : 4,    allowInMatch ? *allowInMatch : true));
		v.push_back(new LinkedListChain(new HashZlib(hashTableSize),  "zlib-9",  chainStrategy>0 ? chainStrategy : 4096, allowInMatch ? *allowInMatch : true));
		v.push_back(new LinkedListChain(new HashPutty(hashTableSize), "putty",   chainStrategy>0 ? chainStrategy : 32,   allowInMatch ? *allowInMatch : true));
		v.push_back(new LinkedListChain(new HashLZJB(hashTableSize),  "lzjb",    chainStrategy>0 ? chainStrategy : 1,    allowInMatch ? *allowInMatch : false));
		v.push_back(new LinkedListChain(new HashLZRW1(hashTableSize), "lzrw1-a", chainStrategy>0 ? chainStrategy : 1,    allowInMatch ? *allowInMatch : false));
		v.push_back(new LinkedListChain(new HashLZO1X(hashTableSize), "lzo1x-1", chainStrategy>0 ? chainStrategy : 1,    allowInMatch ? *allowInMatch : false));
		v.push_back(new LinkedListChain(new HashAF1(hashTableSize),   "af-1",    chainStrategy>0 ? chainStrategy : 1,    allowInMatch ? *allowInMatch : false));
		v.push_back(new NullDictionary());
	} else {
		v.push_back(new DoubleHashTable(new HashZlib(hashTableSize),  "zlib-1",  (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : true));
		v.push_back(new DoubleHashTable(new HashZlib(hashTableSize),  "zlib-9",  (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : true));
		v.push_back(new DoubleHashTable(new HashPutty(hashTableSize), "putty",   (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : true));
		v.push_back(new DoubleHashTable(new HashLZJB(hashTableSize),  "lzjb",    (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : false));
		v.push_back(new DoubleHashTable(new HashLZRW1(hashTableSize), "lzrw1-a", (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : false));
		v.push_back(new DoubleHashTable(new HashLZO1X(hashTableSize), "lzo1x-1", (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : false));
		v.push_back(new DoubleHashTable(new HashAF1(hashTableSize),   "af-1",    (ChainStrategy) chainStrategy,  allowInMatch ? *allowInMatch : false));
		v.push_back(new NullDictionary());
	}

	return v;
}

template<typename T> class DeleteFunctor {
public:	void operator()(T* d) const { delete d; }
};

void cleanupDictionaries(vector<Dictionary*>& v) {
	//for_each(v.begin(), v.end(), [&](Dictionary* d) { delete d; });
	for_each(v.begin(), v.end(), DeleteFunctor<Dictionary>());
}


// This is the smallest match length we care about. Because length+offset
// occupies 3 bytes in the encoding I have in mind, we only care about
// 4 byte+ matches.  (Hash functions etc have a separate assumption that
// at least 4 bytes will always be available)
#define SHORTEST (4)


// Find the longest of all matches for the data in data[needleOffset..length-1].
// If matches are equal-longest then the first found in the vector will be
// returned. The returned value is the vector index of the found match. -1 is
// returned if no good match is found
int bestMatch(const uchar* data, int length, const vector<int>& potentials, int needleOffset)
{
	int bestIndex = -1;
	int bestSize = SHORTEST-1;

	vector<int>::const_iterator p;
	for(p = potentials.begin(); p != potentials.end(); ++p) {
		int matched = 0;
		while(needleOffset+matched<length && data[needleOffset+matched] == data[*p+matched]) {
			++matched;
		}
		if(matched > bestSize) {
			bestIndex = p - potentials.begin();
		}
	}

	return bestIndex;
}

/**
 * Utility function - mirrors the bits in a byte. You can also operate
 * only on the n lowest bits
 */
inline uchar mirrorByte(uchar c, int bits = 8) {
	uchar r = 0;
	for(int i = 0; i < bits; ++i) {
		if(c & (1<<i)) {
			r |= (1 << (bits-1-i));
		}
	}
	return r;
}

/**
 * Deflate implementation with our specific 8-bit-boundary Huffman encoding.
 * Everything in here is pretty much intertwined with the Huffman table defined
 * in deflate_block_header.dat.c
 */
#include "deflate_block_header.dat.c"

#define ENCODED_COPY_BYTES (3)
#define MAX_LENGTH (66)
#define MAX_DISTANCE (1024)
#define APPROX_BLOCK_OVERHEAD (45)   // bytes of overhead to insert a compressed block
#define MAX_UNENCODED_LENGTH (0xffff)

void outputLiteral(vector<uchar>& dest, uchar c) {
	dest.push_back(mirrorByte(c));
}

bool canOutputAsLiteral(uchar c) {
	return c < 128;
}

void outputCopy(vector<uchar>& dest, int distance, int length) {
	// there isn't really a "grand unified theory" of the mapping
	// here, it's just the way the huffman table turns out
	length -= 3;
	distance -= 1;
	assert(length >= 0 && length < 64);
	assert(distance >= 0 && distance < 1024);

	int length_6_bits;
	if     (length < 8)  length_6_bits = 56 + length;
	else if(length < 10) length_6_bits = 48 + mirrorByte(length - 8,  1);
	else if(length < 12) length_6_bits = 50 + mirrorByte(length - 10, 1);
	else if(length < 14) length_6_bits = 52 + mirrorByte(length - 12, 1);
	else if(length < 16) length_6_bits = 54 + mirrorByte(length - 14, 1);

	else if(length < 20) length_6_bits = 32 + mirrorByte(length - 16, 2);
	else if(length < 24) length_6_bits = 36 + mirrorByte(length - 20, 2);
	else if(length < 28) length_6_bits = 40 + mirrorByte(length - 24, 2);
	else if(length < 32) length_6_bits = 44 + mirrorByte(length - 28, 2);

	else if(length < 40) length_6_bits =      mirrorByte(length - 32, 3);
	else if(length < 48) length_6_bits =  8 + mirrorByte(length - 40, 3);
	else if(length < 56) length_6_bits = 16 + mirrorByte(length - 48, 3);
	else if(length < 64) length_6_bits = 24 + mirrorByte(length - 56, 3);
	else { length_6_bits = 0; assert(0); }

	// a bit cleaner here - 1020, 1016, ... is f(x) = 2*[ f(x-1) - 512 ]
	// XXX a few weeks later: or is it?
	int distance_10_bits;
	if     (distance < 4)     distance_10_bits = 1020 + distance;
	else if(distance < 6)     distance_10_bits = 1016 + mirrorByte(distance - 4,   1);
	else if(distance < 8)     distance_10_bits = 1018 + mirrorByte(distance - 6,   1);
	else if(distance < 12)    distance_10_bits = 1008 + mirrorByte(distance - 8,   2);
	else if(distance < 16)    distance_10_bits = 1012 + mirrorByte(distance - 12,  2);
	else if(distance < 24)    distance_10_bits = 992  + mirrorByte(distance - 16,  3);
	else if(distance < 32)    distance_10_bits = 1000 + mirrorByte(distance - 24,  3);
	else if(distance < 48)    distance_10_bits = 960  + mirrorByte(distance - 32,  4);
	else if(distance < 64)    distance_10_bits = 976  + mirrorByte(distance - 48,  4);
	else if(distance < 96)    distance_10_bits = 896  + mirrorByte(distance - 64,  5);
	else if(distance < 128)   distance_10_bits = 928  + mirrorByte(distance - 96,  5);
	else if(distance < 192)   distance_10_bits = 768  + mirrorByte(distance - 128, 6);
	else if(distance < 256)   distance_10_bits = 832  + mirrorByte(distance - 192, 6);
	else if(distance < 384)   distance_10_bits = 512  + mirrorByte(distance - 256, 7);
	else if(distance < 512)   distance_10_bits = 640  + mirrorByte(distance - 384, 7);
	else if(distance < 768)   distance_10_bits =        mirrorByte(distance - 512, 8);
	else if(distance < 1024)  distance_10_bits = 256  + mirrorByte(distance - 768, 8);
	else { distance_10_bits = 0; assert(0); }

	dest.push_back(0xff);
	dest.push_back(mirrorByte((length_6_bits<<2) | (distance_10_bits>>8)));
	dest.push_back(mirrorByte(distance_10_bits & 0xff));
}

void outputEndOfBlock(vector<uchar>& dest) {
	dest.push_back(127); // code 256, huffman encoded
}

void markBlockAsNotLast(uchar* blockHeader) {
	blockHeader[0] &= 254;
}

/**
 * Deflate as much as possible of data[start..size-1] into dest, returning the
 * offset of the first unencoded byte. We stop at the end (return size), or if
 * we encounter a raw byte that can't be encoded in our byte-even format. The
 * block header is NOT written by this function!
 *
 * Note that if you call this multiple times to encode the same input across
 * multiple blocks, you can pass in the same dictionary, and back-references
 * can span data encoded in previous blocks.
 */

class LessThanFunctor {
private: int _n;
public:  LessThanFunctor(int n) : _n(n) { return; }
	 LessThanFunctor(const LessThanFunctor& other) : _n(other._n) { return; }
         bool operator()(int i) const { return i < _n; }
};

int deflateHuffmanBlock(
	vector<uchar>& dest, Dictionary& dictionary, const uchar* data, int start, int size)
{
	int n = start;
	while(n < size-3) {
		vector<int> potentialMatches = dictionary.potentialMatches(data, n);

		potentialMatches.erase(
				remove_if(potentialMatches.begin(),
					potentialMatches.end(),
					LessThanFunctor(n-MAX_DISTANCE)),
				potentialMatches.end());
		
		int pmIdx = bestMatch(data, size, potentialMatches, n);

		if(pmIdx < 0) {
			// drop out now if we've reached a character we can't encode
			if(!canOutputAsLiteral(data[n]))
			break;

			dictionary.insert(data, n, MATCH_NONE);
			if(g_log) fprintf(g_log, "LITERAL:   %c\n", (char) data[n]);
			outputLiteral(dest, data[n]);
			++n;
		} else {
			if(g_log) fprintf(g_log, "COPY:      ");
			int copyOfs = potentialMatches[pmIdx];
			int copyLen = 0;
			int limit = min(size, n+MAX_LENGTH);
			while(n+copyLen < limit && data[n+copyLen] == data[copyOfs+copyLen]) {
				dictionary.insert(data, n+copyLen, copyLen ? MATCH_WITHIN : MATCH_START);
				if(g_log) fprintf(g_log, "%c", (char) data[n+copyLen]);
				++copyLen;
			}
			outputCopy(dest, n - copyOfs, copyLen);
			if(g_log) fprintf(g_log, "     [dist = %0d, len = %0d]\n", n - copyOfs, copyLen);
			n += copyLen;
		}
	}

	while(n < size && canOutputAsLiteral(data[n])) {
		if(g_log) fprintf(g_log, "TRAILER:   %c\n", (char) data[n]);
		outputLiteral(dest, data[n]);
		++n;
	}

	outputEndOfBlock(dest);

	return n;
}

/**
 * Like deflateHuffmanBlock() - but for literal blocks. Presumes that the first char can't
 * be encoded in a compressed block. Returns the number of encoded bytes.
 */
int deflateUnencodedBlockWithHdr(
	vector<uchar>& dest, Dictionary& dictionary, const uchar* data, int start, int size)
{
	// how many bytes to encode?
	int n = 0;

	do {
		// if we're here, we want to encode at least one more byte
		++n;

		// skip over any subsequent badness
		while(start+n < size && !canOutputAsLiteral(data[start+n])) {
			++n;
		}

		// keep going if badness occurs in the next 4 bytes (avoid buffer
		// overrun - the followup APPROX_BLOCK_OVERHEAD check will cover
		// that case anyway)
	} while((start+n+4 < size) && (
		!canOutputAsLiteral(data[start+n+1]) ||
		!canOutputAsLiteral(data[start+n+2]) ||
		!canOutputAsLiteral(data[start+n+3]) ||
		!canOutputAsLiteral(data[start+n+4])));

	// is it obviously pointless to start another block?
	// if less than 100% compression occurs, then the threshold should actually
	// be longer, but this is simple enough for now
	if(start+n+APPROX_BLOCK_OVERHEAD >= size) {
		n = size - start;
	}

	// 64k - 1 is the maximum unencoded block size
	if(n > MAX_UNENCODED_LENGTH) {
		n = 0xffff;
	}

	// output the 3-bit (plug 5-bits padding) block type indicator
	// and the 4-byte block-specific header. the lowest bit of the first
	// byte indicates this is the final block - our caller will fix this up
	// later if required
	dest.push_back(0);
	dest.push_back((uchar) (n & 0xff)); dest.push_back((uchar) ((n>>8) & 0xff)); 
	n ^= 0xffff;
	dest.push_back((uchar) (n & 0xff)); dest.push_back((uchar) ((n>>8) & 0xff)); 
	n ^= 0xffff;

	if(g_log) fprintf(g_log, "UNCODED BLOCK OF %0d CHARACTERS\n", n);

	// output raw data and return
	while(n--) {
		dest.push_back(data[start++]);
	}
	return start;
}

		
/**
 * Creates a gzip file in deflated, from the bytestream in raw,
 * using the supplied dictionary.
 *
 * Prints errors and exit(1)s on I/O errors.
 *
 * Returns false if the given hash isn't valid.
 */
void deflate(vector<uchar>& deflated, const vector<uchar>& raw, Dictionary& dictionary) {
	// gzip header
	deflated.push_back(0x1f); deflated.push_back(0x8b);
	deflated.push_back(0x08); deflated.push_back(0x00);
	deflated.push_back(0x00); deflated.push_back(0x00);
	deflated.push_back(0x00); deflated.push_back(0x00);
	deflated.push_back(0x02); deflated.push_back(0x03);

	// we don't support this (but easily could..)
	assert(raw.size() > 0);

	// actually deflate the data - inserting an unencoded literal
	// block when the deflater can't handle an input byte
	int lastBlockOfs = -1;
	int n = 0;
	while(n < (int) raw.size()) {
		if(lastBlockOfs >= 0) {
			markBlockAsNotLast(&deflated[lastBlockOfs]);
		}
		lastBlockOfs = deflated.size();

		// "dynamic huffman encoded" header, plus huffman table
		for(unsigned int i = 0; i < sizeof deflate_block_header; ++i) {
			deflated.push_back(deflate_block_header[i]);
		}
		n = deflateHuffmanBlock(deflated, dictionary, &raw[0], n, raw.size());

		// can happen more than once if the unencoded data goes for >=64k
		while(n < (int) raw.size() && !canOutputAsLiteral(raw[n])) {
			markBlockAsNotLast(&deflated[lastBlockOfs]);
			lastBlockOfs = deflated.size();

			n = deflateUnencodedBlockWithHdr(deflated, dictionary, &raw[0], n, raw.size());
		}
	}

	// gzip footer
	uint32_t on = htonl(crc((uchar*) &raw[0], raw.size()));
	uchar* onc = (uchar*) &on;
	deflated.push_back(onc[3]); deflated.push_back(onc[2]);
	deflated.push_back(onc[1]); deflated.push_back(onc[0]);

	on = htonl(raw.size());
	deflated.push_back(onc[3]); deflated.push_back(onc[2]);
	deflated.push_back(onc[1]); deflated.push_back(onc[0]);
}


/**
 * Read a file into a vector<uchar>. Screams at the user
 * and exit(1)'s on I/O errors.
 */
void suckFileIn(vector<uchar>& v, FILE* fp) {
	static size_t total = 0;
	static bool warned = 0;

	assert(v.size() == 0);

	while(!feof(fp)) {
		char buf[4096];
		size_t n = fread(buf, 1, sizeof(buf), fp);
		if(ferror(fp)) {
			fprintf(stderr, "I/O error on read, dying!\n");
			exit(1);
		}
		for(size_t i = 0; i < n; ++i) {
			v.push_back(buf[i]);
		}

		total += n;
		if(total > 4*1024*1024 && !warned) {
			warned = true;
			fprintf(stderr, "WARNING: read over 4 megabytes, are we in the right place?\n");
		}
	}
}

void suckFileIn(vector<uchar>& v, const char* filename) {
	FILE* fp = strcmp(filename, "-")
		? fopen(filename, "rb")
		: stdin;

	if(!fp) {
		fprintf(stderr, "couldn't open %s\n", filename);
		exit(1);
	}

	suckFileIn(v, fp);
	fclose(fp);
}


/**
 * Write a vector<uchar> to a file.
 */
void writeFileOut(const vector<uchar>& v, const char* filename) {
	FILE* fp = fopen(filename, "wb");
	if(!fp || v.size() != fwrite(&v[0], 1, v.size(), fp)) {
		fprintf(stderr, "couldn't write to %s\n", filename);
		exit(1);
	}
	fclose(fp);
}



/**
 * Filters stdin to stdout using the supplied hash. Returns
 * false if the given hash is invalid.
 */
bool filterInput(const char* hash) {
	// find the hash
	vector<Dictionary*> dictionaries = makeDictionaries();
	vector<Dictionary*>::const_iterator p;
	for(p = dictionaries.begin(); p != dictionaries.end(); ++p) {
		if(0 == strcmp((*p)->name(), hash)) {
			break;
		}
	}
	if(p == dictionaries.end()) {
		cleanupDictionaries(dictionaries);
		return false;
	}

	// set up source/dest bytestreams
	vector<uchar> raw;
	suckFileIn(raw, stdin);
	vector<uchar> deflated;

	// deflate
	deflate(deflated, raw, **p);

	// write to stdout
	size_t written = fwrite(&deflated[0], 1, deflated.size(), stdout);
	if(ferror(stdout) || written != deflated.size()) {
		fprintf(stderr, "I/O error on stdout, dying!\n");
		exit(1);
	}

	// we're done
	cleanupDictionaries(dictionaries);
	return true;
}


/**
 * Models the behaviour of LZJB. Returns what the given vector would encode to in LZJB
 * (roughly)
 */
size_t lzjbEstimateLength(const vector<uchar>& raw)
{
	LinkedListChain dict(new HashLZJB(), "lzjb", 1, false);
	unsigned int literalBytes = 0;
	unsigned int copies = 0;

	// count possible copies versus literal bytes in the same manner as LZJB
	unsigned int i = 0; 
	while(i < raw.size() - 3) {
		vector<int> m = dict.potentialMatches(&raw[0], i);
		if(m.size() && i-m[0] < 1024 && raw[i]==raw[m[0]] && raw[i+1]==raw[m[0]+1] && raw[i+2]==raw[m[0]+2]) {
			dict.insert(&raw[0], i, MATCH_START);

			int matchLen = 3;
			while(matchLen < 66 && i+matchLen < raw.size() && raw[i+matchLen] == raw[m[0]+matchLen]) {
				++matchLen;
			}

			i += matchLen;
			copies++;
		} else {
			dict.insert(&raw[0], i, MATCH_NONE);

			i++;
			literalBytes++;
		}
	}

	literalBytes += raw.size() - i;

	// add up size of the encoded data
	size_t sz = 8;       // say 8 bytes for size and checksum
	sz += literalBytes;
	sz += 2*copies;
	sz += (literalBytes + copies + 7) / 8;  // control bytes

	return sz;
}


/**
 * analysis(dir) performs an analysis of everything at the given path.
 */
void analysis_dir_recurse(map<const char*, vector<uchar> >& dest,
	const char* dirOpenName,
	const char* dirStoreName)
{
	//fprintf(stderr, "recurse to %s\n", dirOpenName);

	char openName[NAME_MAX + strlen(dirOpenName) + 8];
	strcpy(openName, dirOpenName);
	char* n1 = openName + strlen(openName);
	if(n1 != openName) *(n1++) = '/';

	char storeName[NAME_MAX + strlen(dirStoreName) + 8];
	strcpy(storeName, dirStoreName);
	char* n2 = storeName + strlen(storeName);
	if(n2 != storeName) *(n2++) = '/';

	DIR* d = opendir(dirOpenName);
	if(!d) {
		fprintf(stderr, "Couldn't open %s as a directory\n", openName);
		exit(1);
	}

	// read in files, and store directories to recurse into
	map<string, string> dirs;
	struct dirent* dp;
	while((dp = readdir(d))) {
		strcpy(n1, dp->d_name);
		strcpy(n2, dp->d_name);

		if(dp->d_type == DT_REG) {
			const char* name = strdup(storeName);
			assert(name);

			map<const char*, vector<uchar> >::iterator p =
				dest.insert(make_pair(name, vector<uchar>())).first;

			//fprintf(stderr, "file: %s\n", openName);
			suckFileIn(p->second, openName);
		} else if(dp->d_type == DT_DIR) {
			if(strcmp(n1, ".") != 0 && strcmp(n1, "..") != 0) {
				dirs.insert(make_pair(openName, storeName));
			}
		} else if(dp->d_type == DT_UNKNOWN) {
			fprintf(stderr, "ERROR: readdir is in lala-land on %s\n", openName);
			exit(1);
		} else {
			fprintf(stderr, "WARNING: ignoring %s, unsupported file type\n", openName);
		}
	}
	closedir(d);

	// recurse into subdirectories
	map<string, string>::const_iterator dirp;
	for(dirp = dirs.begin(); dirp != dirs.end(); ++dirp) {
		analysis_dir_recurse(dest, dirp->first.c_str(), dirp->second.c_str());
	}
}

		

void analysis_print(int colWidth, const char* s) {
	int n = printf("%s, ", s) - 2;
	while(n++ < colWidth) printf(" ");
}
	
void analysis_print(int colWidth, int d) {
	char s[40];
	sprintf(s, "%0d", (int) d);
	analysis_print(colWidth, s);
}

void analysis_print(int colWidth, double d) {
	char s[80];
	sprintf(s, "%.1f", d);
	analysis_print(colWidth, s);
}

void analysis_print(int colWidth, size_t d) {
	assert(d >= 0 && d < 100*1024*1024);
	analysis_print(colWidth, (int)d);
}
		

size_t analysis_gzip_test(const vector<uchar>& raw, int level)
{
	assert(level >= 1 && level <= 9);

	// use a couple of temporary files
	static bool init = false;
	static char nameIn[4+L_tmpnam];
	static char nameOut[4+L_tmpnam];
	static char command[40+2*L_tmpnam];
	if(!init) {
		init = true;
		nameIn[0] = 0; tmpnam(nameIn);
		nameOut[0] = 0; tmpnam(nameOut);
	}
	assert(nameIn && nameOut);

	// pass data through gzip via temporary files
	vector<uchar> compressed;
	writeFileOut(raw, nameIn);
	sprintf(command, "gzip -%0d -c < %s > %s", level, nameIn, nameOut);
	if(system(command)) {
		fprintf(stderr, "FAILURE RUNNING: %s\n\n", command);
		exit(1);
	}
	suckFileIn(compressed, nameOut);

	return compressed.size();
}


void analysis_baseline(int namelen, int dictlen, const map<const char*, vector<uchar> >& files)
{
	map<const char*, vector<uchar> >::const_iterator file_p;

	analysis_print(namelen, "File");
	analysis_print(dictlen, "INPUT");
	analysis_print(dictlen, "lzjbemu");
	analysis_print(dictlen, "gzip-1");
	analysis_print(dictlen, "gzip-6");
	analysis_print(dictlen, "gzip-9");
	printf("\n");

	for(file_p = files.begin(); file_p != files.end(); ++file_p) {
		analysis_print(namelen, file_p->first);
		analysis_print(dictlen, file_p->second.size());
		analysis_print(dictlen, lzjbEstimateLength(file_p->second));
		analysis_print(dictlen, analysis_gzip_test(file_p->second, 1));
		analysis_print(dictlen, analysis_gzip_test(file_p->second, 6));
		analysis_print(dictlen, analysis_gzip_test(file_p->second, 9));
		printf("\n");
	}
	printf("\n");
}

void analysis_deflate(bool printHeader,
		int namehdr, int dicthdr,
		const map<const char*, vector<uchar> >& files,
     		int hashTableSize, int chainLength, bool* allowInMatch)
{
	// print header
	int chainhdr = sizeof("ChainStrategy");
	int allowhdr = sizeof("AllowInMatch");
	int hashhdr = sizeof("HashTblSize");
	if(printHeader) {
		analysis_print(chainhdr, "ChainStrategy");
		analysis_print(hashhdr, "HashTblSize");
		analysis_print(allowhdr, "AllowInMatch");
		analysis_print(dicthdr, "Dictionary");
		analysis_print(namehdr, "File");
		analysis_print(dicthdr, "RawSize");
		analysis_print(dicthdr, "Size");
		analysis_print(dicthdr, "PctSize");
		printf("\n");
	}

	// for each file, deflate and print stats	
	map<const char*, vector<uchar> >::const_iterator file_p;
	for(file_p = files.begin(); file_p != files.end(); ++file_p) {
		vector<Dictionary*> dictionaries =
			makeDictionaries(hashTableSize, chainLength, allowInMatch);

		vector<Dictionary*>::const_iterator dictionary_p;
		for(dictionary_p = dictionaries.begin(); dictionary_p != dictionaries.end(); ++dictionary_p) {
			vector<uchar> deflated;
			deflate(deflated, file_p->second, **dictionary_p);

			switch(chainLength) {
			case CHAIN_ALTERNATE:  analysis_print(chainhdr, "Alternate"); break;
			case CHAIN_PROMOTION:  analysis_print(chainhdr, "Promotion"); break;
			case 0:                analysis_print(chainhdr, "Default");   break;
			default:               analysis_print(chainhdr, chainLength); break;
			}
			
			analysis_print(hashhdr, hashTableSize);
			analysis_print(allowhdr, allowInMatch ? (*allowInMatch ? "Yes" : "No") : "Default");
			analysis_print(dicthdr, (*dictionary_p)->name());
			analysis_print(namehdr, file_p->first);
			analysis_print(dicthdr, file_p->second.size());
			analysis_print(dicthdr, deflated.size());
			analysis_print(dicthdr, 100.0 * (double) deflated.size() / (double) file_p->second.size());
			printf("\n");
		}
		cleanupDictionaries(dictionaries);
	}
}

void analysis(const char* path) {
	map<const char*, vector<uchar> > files;
	map<const char*, vector<uchar> >::const_iterator file_p;

	vector<Dictionary*> dictionaries;
	vector<Dictionary*>::const_iterator dictionary_p;

	// is it a directory? if not, treat it as a file
	DIR* d = opendir(path);
	if(d) {
		closedir(d);
		analysis_dir_recurse(files, path, "");
	} else {
		files.insert(make_pair(path, vector<uchar>()));
		suckFileIn(files.begin()->second, path);
	}

	// figure out longest file and dictionary name lengths
	int namelen = 4;
	for(file_p = files.begin(); file_p != files.end(); ++file_p) {
		namelen = max((int) strlen(file_p->first), namelen);
	}

	int dictlen = 10; // 'Dictionary'
	dictionaries = makeDictionaries();
	for(dictionary_p = dictionaries.begin(); dictionary_p != dictionaries.end(); ++dictionary_p) {
		dictlen = max(dictlen, (int) strlen( (*dictionary_p)->name() ));
	}
	cleanupDictionaries(dictionaries);

	//printf("BASELINE\n");
	//analysis_baseline(namelen, dictlen, files);

	const int chainLengths[] = { 0, 1, 2, 3, 4, CHAIN_ALTERNATE, CHAIN_PROMOTION, -1 };
	const int hashTableSizes[] = { 0, 128, 256, 512, 2039, 2048, -1 };
	unsigned int i,j;
	bool tmp;
	bool printHeader = true; // first time only
	for(i = 0; chainLengths[i] != -1; ++i) {
		for(j = 0; hashTableSizes[j] != -1; ++j) {
			int c = chainLengths[i], h = hashTableSizes[j];

			analysis_deflate(printHeader, namelen, dictlen, files, h, c, NULL);
			printHeader = false;
			tmp = true;  analysis_deflate(printHeader, namelen, dictlen, files, h, c, &tmp);
			tmp = false; analysis_deflate(printHeader, namelen, dictlen, files, h, c, &tmp);
		}
	}
}


void usage(bool error) {
	const char* msg = "";
	FILE* fp = stdout;
	if(error) {
		msg = "Invalid command line!\n";
		fp = stderr;
	}

	fprintf(fp, "%s"
"\n"
"Usage: hashtester [-v] {-h | -f dictionary | -a path}\n"
"\n"
"  -v: Enable verbose debugging messages to stderr\n"
"\n"
"  -h: List this help, and available hashes\n"
"  -f: Compress stdin to stdout, using the stated hash\n"
"  -a: Analyse the file at the given path against all dictionaries.\n"
"      You may provide a directory path, in which case we recurse through.\n"
"      \"-a -\" will analyse stdin.\n"
"\n"
"Available hashes:\n", msg);

	vector<Dictionary*> dictionaries = makeDictionaries();
	vector<Dictionary*>::const_iterator p;
	for(p = dictionaries.begin(); p != dictionaries.end(); ++p) {
		printf("    %s\n", (*p)->name());
	}
	printf("\n");
	cleanupDictionaries(dictionaries);
}
	
int main(int argc, char** argv) {
	int ch;
	int vopt=0, hopt=0, fopt=0, aopt=0;
	char* s = "OPTERROR";  /* shut up spurious warning */

	while(-1 != (ch = getopt(argc, argv, "vhf:a:"))) {
		switch(ch) {
		case 'v': vopt++; break;
		case 'h': hopt++; break;
		case 'f': fopt++; s = strdup(optarg); assert(s); break;
		case 'a': aopt++; s = strdup(optarg); assert(s); break;
		default:
			usage(true);
			exit(1);
		}
	}
	argc -= optind;
	argv += optind;

	// no duplicate options, no conflicting options, and exactly one action specified
	if((vopt | hopt | fopt | aopt) > 1 || (hopt + fopt + aopt) != 1) {
		usage(true);
		exit(1);
	}

	// are we logging?
	g_log = vopt ? stderr : NULL;

	// take appropriate action
	if(hopt) {
		usage(false);
	} else if(fopt) {
		if(!filterInput(s)) {
			fprintf(stderr, "Couldn't compress - is %s a valid hash?\n", s);
			exit(1);
		}
	} else if(aopt) {
		analysis(s);
	}

	return 0;
}


