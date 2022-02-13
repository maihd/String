CC=gcc
EXE=StringTest

CFLAGS=-Wall -Wextra -Wwrite-strings -Wredundant-decls -Wno-unused-parameter

ifeq (Windows_NT,$(OS))
EXE=StringTest.exe
endif

all:
	$(CC) -o $(EXE) StringTest.c String.c $(CFLAGS)

cpp:
	$(CC) -o $(EXE) StringTest.cpp String.cpp $(CFLAGS)

safe:
	$(CC) -o $(EXE) string_safe_test.c $(CFLAGS)

clean: 
	rm -f StringTest
	rm -f StringTest.exe