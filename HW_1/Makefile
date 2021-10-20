main: 
	gcc main.c -o main 

test: 
	gcc test.c -o test

.PHONY : clean
clean:
	rm -rf *.o main
	rm -rf *.o test
	rm -rf ./fifo/main_pipe ./fifo/connection_pipe