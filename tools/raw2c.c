
// expects a single structure name on the command line
// converts raw binary on stdin to a C array on stdout
//
// example:
//  $ echo -n hello | ./raw2c foobar
//  
//  unsigned char foobar[] = {
//   104,
//   101,
//   108,
//   108,
//   111
//  };
//

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv) {
	assert(argc == 2);

	printf("\n");
	printf("unsigned char %s[] = {\n", argv[1]);
	
	int c = fgetc(stdin);
	assert(!ferror(stdin));
	assert(c != EOF);
	printf(" %0d", c);

	while(EOF != (c = fgetc(stdin))) {
		assert(!ferror(stdin));
		printf(",\n %0d", c);
	}

	printf("\n};\n\n");

	assert(!ferror(stdout));

	return 0;
}



