
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
#include <ctype.h>
#include <string.h>

int g_line;

void warn_if_digits(const char* walker)
{
	while(isspace(*walker)) {
		++walker;
	}
	if(isdigit(*walker)) {
		fprintf(stderr, "LINE %0d: comment starts with a digit, could be badly placed data\n", g_line);
	}
}

int main(int argc, char**argv) {
	int byte = 0;
	int bits = 0;

	int c, i;
	char line[1024];

	assert(argc < 3);

	FILE* fp = stdin;
	if(argc == 2) {
		fp = fopen(argv[1], "rb");
	}
	assert(fp);
	
	g_line = 0;
	while(fgets(line, 1023, fp)) {
		++g_line;
		assert(strlen(line) < 1000);

		const char* c = line;
		while(isspace(*c)) {
			++c;
		}
		if(*c >= '0' && *c <= '1' && (c - line) > 8) {
			fprintf(stderr, "LINE %0d: tons of space before the first digit, maybe intended as a comment?\n", g_line);
		}
		while(*c == '0' || *c == '1') {
			++c;
		}

		warn_if_digits(c);

		--c;
		while(c >= line && isdigit(*c)) {
			if(*c == '1') {
				byte |= (1<<bits);
			}
			++bits;
			if(bits == 8) {
				fputc(byte, stdout);
				byte = 0;
				bits = 0;
			}

			--c;
		}
	}
	assert(!ferror(fp));
	
	if(bits != 0) {
		fputc(byte, stdout);
		fprintf(stderr, "WARNING: only %0d bits present in last byte\n",
				bits);
	}
			
	return 0;
}


