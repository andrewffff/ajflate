all: encode evenbytes bindump gzdump raw2c hashtester

clean:
	rm encode evenbytes bindump gzdump raw2c hashtester deflate_block_header.dat.c

encode: encode.c
	gcc -g encode.c -o encode

evenbytes: evenbytes.c
	gcc -g evenbytes.c -o evenbytes

bindump: bindump.c
	gcc -g bindump.c -o bindump

gzdump: gzdump.c huffman.h
	gcc -g gzdump.c -o gzdump

raw2c: raw2c.c
	gcc -g raw2c.c -o raw2c

hashtester: hashtester.cpp intarray.h linkedlistchain.h doublehashtable.h deflate_block_header.dat.c
	g++ -g -O1 -Wall hashtester.cpp -o hashtester

deflate_block_header.dat.c: encode raw2c deflate_block_header.in
	./encode deflate_block_header.in | ./raw2c deflate_block_header > deflate_block_header.dat.c

