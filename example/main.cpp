// converts plaintext stdin to gzipped stdout

#include "ajflate.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

std::vector<unsigned char> suckFileIn(FILE* fp) {
	std::vector<unsigned char> v;

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
	}
	return v;
}

int main(int argc, char** argv) {
	std::vector<unsigned char> srcv = suckFileIn(stdin);
	
	unsigned int size;
	unsigned char* content = make_gzip(&size, &srcv[0], srcv.size());

	if(!content) {
		fprintf(stderr, "got NULL pointer back from make_gzip - out of memory? aborting\n");
		exit(1);
	}

	size_t written = fwrite(content, 1, size, stdout);
	if(written != size) {
		fprintf(stderr, "could not write out all content, aborting\n");
		exit(1);
	}
	return 0;
}

