obj_main = main.o

main.o: main.c
	gcc -c main.c

main: $(obj_main)
	gcc -o main $(obj_main)


file_copy.o: file_copy.c
	gcc -c file_copy.c

file_copy: file_copy.o
	gcc -o file_copy file_copy.o


.PHONY : clean
clean:
	rm -rf *.o main
	rm -rf *.o file_copy
	rm -rf *.o file_copy_debug

.PHONY : debud
debug: file_copy.c
	gcc -g file_copy.c -o file_copy_debug
	gdb file_copy_debug
