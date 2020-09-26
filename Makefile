CC=cc
EXE=StringTest

ifeq (Windows_NT,$(OS))
EXE=StringTest.exe
endif

all:
	$(CC) -o $(EXE) StringTest.c String.c -Wall -Wextra

safe:
	$(CC) -o $(EXE) string_safe_test.c -Wall -Wextra

clean: 
	rm -f StringTest
	rm -f StringTest.exe