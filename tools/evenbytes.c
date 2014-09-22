
#include <stdio.h>
#include <assert.h>
#include "../contrib/putty_data.h"

int main(int argc, char** argv) {
	// with the fixed huffman table, all the codes that represent
	// lengths are 7 bits long, and all the codes that represent
	// distances are 5 bits long. So it's always 12 bits, plus any
	// "extra bits" for length or distance
	int lengthCode, distanceCode;

	for(lengthCode = 257; lengthCode <= 285; ++lengthCode) {
		for(distanceCode = 0; distanceCode <= 29; ++distanceCode) {
			const coderecord* len = find_code(lencodes, lengthCode);
			const coderecord* dist = find_code(distcodes, distanceCode);

			int bits = 12 + len->extrabits + dist->extrabits;
			if(0 == (bits%8)) {
				printf("%0d\t%0d-%0d\t%0d-%0d\n",
					bits,
					len->min, len->max,
					dist->min, dist->max);
			}
		}
	}
	return 0;
}

