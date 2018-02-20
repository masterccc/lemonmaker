all: bar i3_ipc_test

i3_ipc_test: tests/i3_ipc_test.o
	gcc -o tests/i3_ipc_test tests/i3_ipc_test.o

i3_ipc_test.o: tests/i3_ipc_test.c
	gcc -c tests/i3_ipc_test.c

bar: bar.o
	gcc -o bar bar.o -ansi -pedantic

bar.o: bar.c bar.h
	gcc -c bar.c -ansi -pedantic

clean:
	rm -f *.o bar tests/i3_ipc_test
