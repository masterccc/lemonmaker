bar.o: bar.c
	gcc -c bar.c
bar: bar.o
	gcc -o bar bar.o
clean:
	rm -f *.o bar
