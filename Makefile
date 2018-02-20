all: bar

bar: bar.o
	gcc -o bar bar.o -ansi -pedantic

bar.o: bar.c bar.h
	gcc -c bar.c -ansi -pedantic

clean:
	rm -f *.o bar
