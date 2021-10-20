main.o: main.c
	gcc -c main.c

main: main.o
	gcc -o main main.o 

test.o: test.c
	gcc -c test.c

test: test.o
	gcc -o test test.o

.PHONY : clean
clean:
	rm -rf *.o main
	rm -rf *.o test