CC=cc
EXE=StringTest

CFLAGS=-Wall -Wextra -Wwrite-strings -Wstrict-prototypes -Wredundant-decls -Wno-unused-parameter

ifeq (Windows_NT,$(OS))
EXE=StringTest.exe
endif

all:
	$(CC) -o $(EXE) StringTest.c String.c $(CFLAGS)

safe:
	$(CC) -o $(EXE) string_safe_test.c $(CFLAGS)

clean: 
	rm -f StringTest
	rm -f StringTest.exe