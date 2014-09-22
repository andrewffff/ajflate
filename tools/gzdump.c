
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../contrib/putty_data.h"
#include "huffman.h"

// Buffer to track what we're decompressing :)
char window[65536];
int windowofs = 0;

// The "fixed" huffman table
static HuffSymbol huffman_fixed_ll[288];
static HuffSymbol huffman_fixed_distance[30];

static void huffman_fixed_init()
{
	int i;

	memset(huffman_fixed_ll, 0, sizeof(huffman_fixed_ll));
	for(i =   0; i <= 143; ++i) huffman_fixed_ll[i].bits = 8;
	for(i = 144; i <= 255; ++i) huffman_fixed_ll[i].bits = 9;
	for(i = 256; i <= 279; ++i) huffman_fixed_ll[i].bits = 7;
	for(i = 280; i <= 287; ++i) huffman_fixed_ll[i].bits = 8;
	huffman_assign_codes(huffman_fixed_ll, 288);

	memset(huffman_fixed_distance, 0, sizeof(huffman_fixed_distance));
	for(i = 0; i <= 29; ++i) huffman_fixed_distance[i].bits = 5;
	huffman_assign_codes(huffman_fixed_distance, 30);
}

//
//
// Pull bits out of files. You can also push bits back onto the
// input stream. Maximum of 16 bits per op.
//
//

// get() will leave up to 23 bits in the cache, and put_back can
// append another 16 bits. 23+16 > 32, so use long long
FILE* fp;
long long int bitcache = 0;
int bitsleft = 0;

int peek(int bits)
{
	int c;
	while(bitsleft < 16 && (c=fgetc(fp)) != EOF) {
		bitcache |= ((long long int)c)<<bitsleft;
		bitsleft += 8;
	}

	assert(bits <= 16 && bits <= bitsleft);

	return (int)(bitcache & ((1LL<<bits)-1LL));
}

int get(int bits) {
	int c = peek(bits);
	bitcache >>= bits;
	bitsleft -= bits;
	return c;
}


void put_back(int data, int bits)
{
	assert(bits <= 16);
	assert(bits + bitsleft < 40);

	data &= (1<<bits)-1;

	bitcache <<= bits;
	bitcache |= (long long int)data;

	bitsleft += bits;
}

int byte_remainder()
{
	peek(0);
	return bitsleft % 8;
}


int get_eof()
{
	peek(0);
	return bitsleft == 0;
}


//
// utilities to reverse and debug-dump bitsets
//
int reverse(int data, int bits) {
	int forward = data;
	int backwards = 0;

	int forwardMask = 1;
	int backMask = 1 << (bits-1);

	while(bits-- > 0) {
		if(forward & forwardMask) {
			backwards |= backMask;
		}
		forwardMask <<= 1;
		backMask >>= 1;
	}

	return backwards;
}


void dump(int data, int bits, const char* comment, ...)
{
	int spaces = 17;
	assert(bits <= 16);

	while(bits-- > 0) {
		fputc((data & (1<<bits)) ? '1' : '0', stdout);
		spaces--;
	}

	while(spaces-- > 0) {
		fputc(' ', stdout);
	}

	if(comment) {
		va_list args;
		va_start(args, comment);
		vprintf(comment, args);
		va_end(args);
	}
	
	printf("\n");
}

//
// a deflate decoder which hardcodes constants derived from the "fixed"
// huffman table
//
void decode_fixed_hardcoded()
{
	char msg[64];

	int code = 0;
	while(code != 256) {
		int bits = 7;
		int raw = get(7);
		int data = reverse(raw, bits);

		if(data <= 23) {
			// 7 bits: 0000000 thru 0010111
			code = 256 + data;
		} else {
			raw |= get(1) << bits;
			bits++;
			data = reverse(raw, bits);

			if(data <= 0xBF) {
				// 8 bits: 00110000 thru 10111111
				code = data - 48;
			} else if (data >= 0xC0 && data <= 0xC7) {
				// 8 bits: 11000000 thru 11000111
				code = 280 + data - 0xC0;
			} else {
				// 9 bits: 110010000 thru 111111111
				raw |= get(1) << bits;
				bits++;
				data = reverse(raw, bits);

				code = 144 + data - (0xC8 << 1);
			}
		}

		if(code <= 256) {
			if(code == 256) {
				sprintf(msg, "END OF BLOCK");
			} else if(isprint(code)) {
				sprintf(msg, "literal: %0d  '%c'", code, code);
			} else {
				sprintf(msg, "literal: %0d", code);
			}
			dump(raw, bits, msg);
		} else {
			const coderecord* cr;
			cr = find_code(lencodes, code);

			sprintf(msg, "copy code: %0d = length [%0d..%0d]",
				code, cr->min, cr->max);
			dump(raw, bits, msg);

			raw = get(cr->extrabits);
			sprintf(msg, "#   %0d extra bits: len = %0d",
				cr->extrabits,
				cr->min + raw);
			dump(raw, cr->extrabits, msg);

			raw = get(5);
			data = reverse(raw, 5);
			cr = find_code(distcodes, data);
			sprintf(msg, "  distance code: %0d [%0d..%0d]",
				data, cr->min, cr->max);
			dump(raw, 5, msg);

			raw = get(cr->extrabits);
			sprintf(msg, "  # %0d extra bits: distance = %0d",
				cr->extrabits,
				cr->min + raw);
			dump(raw, cr->extrabits, msg);
		}
	}
}

//
// a decoder that functions on in-memory dynamic huffman data
//

void decode_with_huffman(HuffSymbol* huff_ll, int count_ll, HuffSymbol* huff_dist, int count_dist)
{
	char msg[64];
	int* lookup_ll   = huffman_create_lookup(huff_ll, count_ll);
	int* lookup_dist = huffman_create_lookup(huff_dist, count_dist);

	int code = 0;
	while(code != 256) {
		int raw = get(16);
		int data = reverse(raw, 16);
		code = lookup_ll[data];
		if(code < 0) {
			dump(raw, 16, "DATA DOESN'T JIBE WITH HUFFMAN TABLE");
			exit(1);
		}

		int bits = huff_ll[code].bits;

		put_back(raw, 16);
		raw = get(bits);
		data = reverse(raw, bits);

		if(code <= 256) {
			if(code == 256) {
				dump(raw, bits, "END OF BLOCK");
			} else if(isprint(code)) {
				dump(raw, bits, "literal: %0d  '%c'", code, code);
			} else {
				dump(raw, bits, "literal: %0d", code);
			}
			window[(windowofs++) & 0xffff] = code;
		} else {
			const coderecord* cr;
			cr = find_code(lencodes, code);

			dump(raw, bits, "copy code: %0d = length [%0d..%0d]",
				code, cr->min, cr->max);

			raw = get(cr->extrabits);
			int length = cr->min + raw;
			dump(raw, cr->extrabits,
				"#   %0d extra bits: len = %0d",
				cr->extrabits,
				length);

			raw = get(16);
			data = reverse(raw,16);
			code = lookup_dist[data];
			bits = huff_dist[code].bits;
			put_back(raw,16);
			raw = get(bits);
			data = reverse(raw,bits);

			cr = find_code(distcodes, code);
			dump(raw, bits, "  distance code: %0d [%0d..%0d]",
				code, cr->min, cr->max);

			raw = get(cr->extrabits);

			int distance = cr->min + raw;
			char copystr[260];
			int ci = 0;
			while(ci < length) {
				char c = window[(windowofs - distance + ci) & 0xffff];
				copystr[ci] = isalnum(c) ? c : '.';
				window[(windowofs + ci) & 0xffff] = c;

				++ci;
			}
			copystr[length] = '\0';
			windowofs += length;


			dump(raw, cr->extrabits,
				"  # %0d extra bits: distance = %0d (\"%s\")",
				cr->extrabits,
				distance,
				copystr);
		}
	}

	free(lookup_ll);
	free(lookup_dist);
}





//
// top-level deflate block processing
//
void process_fixed_huffman()
{
	// should yield the same results
	//decode_fixed_hardcoded();

	decode_with_huffman(huffman_fixed_ll, 288, huffman_fixed_distance, 30);
}


// create one of the two man huffman definitions from the huffman-encoded
// metadescription at the front of a block..
void decode_meta_huffman(HuffSymbol* metahuff, int* metalookup, 
		HuffSymbol* dest, int destlen, const char* destname)
{
	memset(dest, 0, sizeof(*dest) * destlen);
	

	int i = 0;
	printf("    # destlen = %0d\n", destlen);
	while(i < destlen) {
		int s = metalookup[reverse(peek(16),16)];
		assert(s >= 0);
		int bits = metahuff[s].bits;
		int raw = get(bits);

		if(s < 16) {
			dump(raw, bits, "  %s[%0d].bits = %0d", destname, i, s);
			dest[i++].bits = s;
		} else if(s == 16) {
			int lastBits = dest[i-1].bits;

			dump(raw, bits, "(prefix: dup previous code 3-6 times");
			int extra = get(2);
			int last = i + 3 + extra - 1;
			dump(extra, 2, "  %s[%0d..%0d].bits = %0d",
				destname, i, last, lastBits);
			while(i <= last && i < destlen) dest[i++].bits = lastBits;
		} else if(s == 17) {
			dump(raw, bits, "(prefix: absent symbols 3-10 times)");
			int extra = get(3);
			int last = i + 3 + extra - 1;
			dump(extra, 3, "  %s[%0d..%0d].bits = 0",
				destname, i, last);
			while(i <= last && i < destlen) dest[i++].bits = 0;
		} else if(s == 18) {
			dump(raw, bits, "(prefix: absent symbols 11-138 times)");
			int extra = get(7);
			int last = i + 11 + extra - 1;
			dump(extra, 7, "  %s[%0d..%0d].bits = 0",
				destname, i, last);
			while(i <= last && i < destlen) dest[i++].bits = 0;
		}
	}
	assert(i == destlen);
}

void process_dynamic_huffman()
{
	int i;
	int data;
	int bits;
	int s;

	int hlit = 257 + (data = get(5));
	dump(data, 5, "HLIT = 257 + %0d = %0d", data, hlit);

	int hdist = 1 + (data = get(5));
	dump(data, 5, "HDIST = 1 + %0d = %0d", data, hdist);
	assert(hdist <= 30); // possible in real world?

	int hclen = 4 + (data = get(4));
	dump(data, 4, "HCLEN = 4 + %0d = %0d", data, hclen);

	HuffSymbol metahuff[19];
	memset(metahuff, 0, sizeof(metahuff));
	for(i = 0; i < hclen; ++i) {
		data = get(3);
		dump(data, 3, "metahuff[%0d] = %0d", metahuffcodes[i], data);
		metahuff[metahuffcodes[i]].bits = data;
	}

	huffman_assign_codes(metahuff, 19);
	printf("  META HUFFMAN CODES:\n");
	huffman_dump(metahuff, 19);
	int* lookup = huffman_create_lookup(metahuff, 19);

	HuffSymbol huff_ll_dist[288+32]; // maximum hlit+hdist
	decode_meta_huffman(metahuff, lookup, huff_ll_dist, hlit+hdist, "huff_ll_dist");

	free(lookup);

	huffman_assign_codes(huff_ll_dist, hlit);
	printf("  LITERAL/LENGTH HUFFMAN CODES:\n");
	huffman_dump(huff_ll_dist, hlit);

	huffman_assign_codes(huff_ll_dist+hlit, hdist);
	printf("  DISTANCE HUFFMAN CODES:\n");
	huffman_dump(huff_ll_dist+hlit, hdist);

	decode_with_huffman(huff_ll_dist, hlit,
			huff_ll_dist+hlit, hdist);
}


void process_uncompressed()
{
	int i, j;

	i = byte_remainder();
	dump(get(i), i, "Junk bits to return to byte alignment");

	i = get(16);
	dump(i, 16, "Length of uncompressed block");		
	j = get(16);
	dump(j, 16, "XOR(ffff) of the length");
	assert((i ^ 0xFFFF) == j);

	while(i--) {
		j = get(8);
		if(isprint(j)) {
			dump(j, 8, "uncompressed: %0d  '%c'", j, j);
		} else {
			dump(j, 8, "uncompressed: %0d", j);
		}
	}
}



int main(int argc, char**argv) {
	int i, j;

	huffman_fixed_init();

	assert(argc < 3);
	fp = stdin;
	if(argc == 2) {
		fp = fopen(argv[1], "rb");
	}
	assert(fp);

	for(i = 0; i < 10; ++i) {
		dump(get(8), 8, "gzip_header[%0d]", i);
	}

	int final = 0;
	int blockCount = 0;
	while(!final) {
		final = get(1);
		dump(final, 1, "START BLOCK #%0d %s",
			blockCount,
			final ? "(final)" : "(more to come)");
		blockCount++;

		int ctype = get(2);
		switch(ctype) {
		case 0:
			dump(0, 2, "format: uncompressed");
			process_uncompressed();
			break;
		case 1:
			dump(1, 2, "format: compressed, fixed huffman");
			process_fixed_huffman();
			break;
		case 2:
			dump(2, 2, "format: compressed, dynamic huffman");
			process_dynamic_huffman();
			break;
		case 3:
			dump(3, 2, "format: undefined ERROR ERROR");
			assert(!"encountered undefined encoding");
			break;
		}
	}

	i = byte_remainder();
	dump(get(i), i, "Junk bits to return to byte alignment");

	for(i = 0; i < 4; ++i) {
		dump(get(8), 8, "crc[%0d]", i);
	}

	for(i = 0; i < 4; ++i) {
		dump(get(8), 8, "decompressed_size[%0d]", i);
	}

	while(!get_eof()) {
		dump(get(8), 8, "JUNK AT END OF FILE");
	}



	return 0;
}
