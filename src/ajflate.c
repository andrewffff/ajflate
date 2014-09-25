
#include "../contrib/crc32.h"
#include <stdint.h>
#include <arpa/inet.h>   // htonl
#include <assert.h>
#include <stdlib.h>
#include <string.h>  // memcpy
#include <limits.h>  // UINT_MAX

/* magic strings of bytes */
#include "deflate_block_header.dat.c"
#include "deflate_length_lookup.dat.c"
#include "deflate_literal_lookup.dat.c"
#include "deflate_distance_lookup.dat.c"
const unsigned char gzip_header[] = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03 };


#define HASH(p) (((257*(p)[0] + 263*(p)[1] + 269*(p)[2]) % 2039) % 256)
#define HASH_MAX (256)

#define CAN_ENCODE_BYTE(b) (((b) & 0x80) == 0)

#define MAX_UNENCODED_LENGTH (0xffff)

#define MIN_MATCH_LENGTH (4)
#define MAX_MATCH_LENGTH (66)

#define MAX_MATCH_DISTANCE (1024)

#define MIN_DISTANCE_CODE (1)
#define MIN_LENGTH_CODE (3)

#define MIN2(a,b) ((a) < (b) ? (a) : (b))
#define MAX2(a,b) ((a) > (b) ? (a) : (b))

#define NOINLINE __attribute__ ((noinline))


static inline void write32LE(unsigned char* d, uint32_t n) {
	d[0] = n & 0xFF;
	d[1] = (n >> 8) & 0xFF;
	d[2] = (n >> 16) & 0xFF;
	d[3] = (n >> 24);
}

static inline void markNotLast(unsigned char* blockHeader) {
	*blockHeader &= 0xFE;
}

// compress src[srcOfs..srcSize+1] into *dest
//
// at least srcSize - srcOfs bytes must be available in *dest
// *dest will be advanced appropriately to the first unwritten byte
// in the output buffer
//
// the first unwritten source offset will be returned. this may be srcSize,
// or may be within the input range iff an uncompressible byte is encountered
// the output buffer must have at least srcSize bytes available
//    (the output will NEVER expand over the input size by more than the size of the header, plus one)
//
// 
static unsigned int compressedFragment(unsigned char** dest, const unsigned char* src, unsigned int srcSize) {
	unsigned int lookback[HASH_MAX];

	unsigned int s = 0;
	unsigned char *d = *dest + sizeof(deflate_block_header);
	while(s < srcSize && CAN_ENCODE_BYTE(src[s])) {
		unsigned int h = HASH(src + s);
		unsigned int sprev = lookback[h];
		lookback[h] = s;

		if(sprev < s && sprev >= s - MAX_MATCH_DISTANCE) {
			unsigned int maxlen = MIN2(srcSize - s, MAX_MATCH_LENGTH);
			unsigned int len;

			for(len = 0;
				len < maxlen && src[s + len] == src[sprev + len];
				len++)
			{;}

			if(len >= MIN_MATCH_LENGTH) {
				unsigned int dval = s - sprev - MIN_DISTANCE_CODE;
				*(d++) = 0xFF;
				*(d++) = deflate_distance_lookup[dval*2] | deflate_length_lookup[len - MIN_LENGTH_CODE];
				*(d++) = deflate_distance_lookup[dval*2 + 1];
				s += len;
				continue;
			}
		}

		*(d++) = deflate_literal_lookup[src[s]];
		s += 1;
	}

	// epilogue
	*(d++) = 127;

	// prologue
	memcpy(*dest, deflate_block_header, sizeof(deflate_block_header));
	if(s != srcSize)
		markNotLast(*dest);

	*dest = d;
	return s;
}

static unsigned int uncompressedFragment(unsigned char** dest, const unsigned char* src, unsigned int srcSize) {
	const unsigned int maxlen = MIN2(srcSize, MAX_UNENCODED_LENGTH);

	// In input which ranges over all 256 possible bytes, rapid flip-flopping between deflated blocks
	// and incompressed blocks creates truly impractical bloat - most of the output is block headers.
	// to avoid this, we scan ahead by an arbitary amount (some small multiple of the header size seems
	// to make sense) and if there are more bytes out ahead that can't go into a compressed block, we
	// remain in non-compressed mode.
	//
	// We could probably tune this threshold, and it's also tempting to make the copy concurrent with the
	// scan-ahead to use more memory bandwidth and avoid making large blocks take two trips through the
	// L1 cache. But the max size is only 64k and we're on the record as performing badly when non-7-bit
	// values get involved, so we'll settle for limiting the downside and maybe optimize Later (tm)
	unsigned int len = 1;
	unsigned int scanahead = len;
	while(scanahead < maxlen && scanahead - len < 2*sizeof(deflate_block_header)) {
		if(!CAN_ENCODE_BYTE(src[scanahead]))
			len = scanahead;
		scanahead++;
	}
	
	(*dest)[0] = 0;
	if(len != srcSize)
		markNotLast(*dest);
	(*dest)[1] = len & 0xFF;
	(*dest)[2] = len >> 8;
	(*dest)[3] = ~(*dest)[1];
	(*dest)[4] = ~(*dest)[2];

	memcpy(5 + *dest, src, len);

	*dest += 5 + len;
	return len;
}

// we may read up to a few bytes past src[srcSize-1]
#ifdef __cplusplus
extern "C"
#endif
unsigned char* make_gzip(unsigned int* sizeOut, const unsigned char* src, unsigned int srcSize) {
	// gzip header, plus deflate block prologue/epilogue, plus uncompressed block prologue, plus gzip footer, plus error margin of 32
	const unsigned int required_buffer = 8 + sizeof(deflate_block_header) + 1 + 5 + 8 + 32;

	unsigned int destSize = srcSize + required_buffer + 128;
	unsigned char* dest = malloc(destSize);
	if(!dest) return NULL;

	memcpy(dest, gzip_header, sizeof(gzip_header));
	unsigned char* d = dest + sizeof(gzip_header);
	
	unsigned int s = 0;
	do {
		if(destSize - (d-dest) < srcSize - s + required_buffer) {
			unsigned char* new_dest = 0;
			if(destSize < UINT_MAX/2) {
				// avoid overflow
				destSize = MAX2(destSize*2, srcSize - s + required_buffer);
			   	new_dest = realloc(dest, destSize);
			}
			if(!new_dest) {
				free(dest);
				return NULL;
			}
			d = new_dest + (d - dest);
			dest = new_dest;
		}

		if(s < srcSize && !CAN_ENCODE_BYTE(src[s]))
			s += uncompressedFragment(&d, src + s, srcSize - s);
		if(s < srcSize && CAN_ENCODE_BYTE(src[s]))
			s += compressedFragment(&d, src + s, srcSize - s);
	} while(s < srcSize);

	// gzip footer
	write32LE(d, crc((unsigned char*)src, srcSize));
	write32LE(d+4, srcSize);
	d += 8;

	*sizeOut = d - dest;
	return dest;
}

	

	

