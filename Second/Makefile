third:
	rm -rf *.o third
	gcc third.c -o third -l pthread

main: 
	gcc main.c -o main 

second:
	rm -rf *.o second
	gcc second.c -o second 

.PHONY : clean
clean:
	rm -rf *.o first
	rm -rf *.o second
	rm -rf *.o third