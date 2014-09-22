
// "encode" and "bindump" are quick and dirty tools to translate between raw binary data,
// and text files full of ones and zeros (with optional comments)
//
// "bindump" takes binary on stdin and outputs a text file. "encode" does the opposite
//
// For instance, "echo Hi > hi.txt" produces a file with three bytes - 'H' (hex 48),
// 'i' (hex 69) and a newline (hex 0A). From this, bindump would produce:
//
//   01001000
//   01101001
//   00001010
//
// You can include comments - anything on a line following a non-binary character will
// be ignored. You don't need to put 8 bits on a line, and you can put spacing in between
// your binary digits. So the following would represent the same data:
//
//   01001000  Hex 48
//   0110      Hex 6
//     1001    Hex 9
//   00 0010 10  Hex 0A
//
// The encode tool will recreate the three-byte 'Hi' file from either of the above.

#include <stdio.h>
#include <assert.h>

int main(int argc, char**argv) {
	int c, i;

	assert(argc < 3);

	FILE* fp = stdin;
	if(argc == 2) {
		fp = fopen(argv[1], "rb");
	}
	assert(fp);

	while((c = fgetc(fp)) != EOF) {
		for(i = 0; i < 8; ++i) {
			fputc((c & 0x80) ? '1' : '0', stdout);
			c <<= 1;
		}
		fputc('\n', stdout);
	}

	return 0;
}


