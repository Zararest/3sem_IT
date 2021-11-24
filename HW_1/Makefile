main.o: main.c
	gcc -c main.c

main: main.o
	gcc -o main main.o 

distinguish_fd.o: distinguish_fd.c
	gcc -c distinguish_fd.c

dist_fd: distinguish_fd.o
	gcc -o dist_fd distinguish_fd.o

test.o: test.c
	gcc -c test.c

test_2.o: test_2.c
	gcc -c test_2.c

test: test.o test_2.o
	gcc -o test test.o
	gcc -o test2 test_2.o

.PHONY : clean
clean:
	rm -rf *.o main
	rm -rf *.o test test2
	rm -rf dist_fd
