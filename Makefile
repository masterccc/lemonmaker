all: bar

bar: bar.o
	gcc -o bar bar.o

bar.o: bar.c bar.h
	gcc -c bar.c

clean:
	rm -f *.o bar
