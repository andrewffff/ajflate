
OBJS = main.o ajflate.o

example: $(OBJS)
	g++ -o $@ $^

clean:
	rm example $(OBJS)

main.o: main.cpp
	g++ -I../src -c -o $@ $<

ajflate.o: ../src/ajflate.c
	gcc -std=c99 -O3 -c -o $@ $<

