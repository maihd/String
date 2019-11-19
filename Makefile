all:
	gcc -o StringTest StringTest.c -Wall -Wextra

clean: 
	rm -f StringTest
	rm -f StringTest.exe