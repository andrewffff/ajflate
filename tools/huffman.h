//
//
// huffman table infrastructure
//
//


// .code is filled from bit15 down. So, if a particular symbol
// is represented by 1011, bits = 4, code = 0xB000
typedef struct {
	int bits;
	int code;
} HuffSymbol;

static void huffman_assign_codes(HuffSymbol*, int);
static int* huffman_create_lookup(HuffSymbol *symbols, int n);
static void huffman_dump(HuffSymbol*, int);

static void huffman_assign_codes(HuffSymbol *symbols, int n) {
	int code = 0;
	int increment = 1<<15;
	int bits = 1;

	while(bits < 16) {
		assert(increment > 0);

		int i;
		for(i = 0; i < n; ++i) {
			if(symbols[i].bits == bits) {
				assert(code < 0x10000);
				symbols[i].code = code;
				code += increment;
			}
		}

		bits++;
		increment >>= 1;
	}
	assert(code <= 0x10000);
}

// create a lookup table from a list of symbols that have been
// through huffman_assign_codes
static int* huffman_create_lookup(HuffSymbol *symbols, int n)
{
	int* lookup = malloc(sizeof(int) * 65536);
	assert(lookup);
	memset(lookup, 0xff, sizeof(int) * 65536);

	int i;
	for(i = 0; i < n; ++i) {
		if(symbols[i].bits > 0) {
			int first = symbols[i].code;
			int last = symbols[i].code + (0x10000 >> symbols[i].bits) - 1;
			int lidx;
			for(lidx = first; lidx <= last; ++lidx) {
				if(lookup[lidx] != -1) {
					printf("lookup build failed on symbol #%0d\n", lidx);
					assert(lookup[lidx] == -1);
				}
				lookup[lidx] = i;
			}
		}
	}

	return lookup;
}


static void huffman_dump(HuffSymbol* symbols, int n) {
	int columns = 4;
	int rows = (n+3)/columns;

	int r;
	for(r = 0; r < rows; ++r) {
		int i;
		for(i = r; i <= r+3*rows && i < n; i += rows) {
			printf("[%3d]  ", i);
			int bit;
			for(bit = 0; bit < 16; ++bit) {
				if(bit >= symbols[i].bits) {
					printf(" ");
				} else {
					printf(symbols[i].code & (0x8000>>bit) ? "1" : "0");
				}
			}

		}
		printf("\n");
	}
}

