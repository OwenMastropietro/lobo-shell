# Directories
SRC = src
INC = include
OBJ = obj
BIN = bin

# Compiler
CFLAGS = -I$(INC) -Wall -Wextra -Werror -pedantic -g
LDFLAGS = 
CC = gcc
SHELL = bash


# Targets
all : lobo_shell.x

lobo_shell.x : main.o parsetools.o builtins.o
	$(CC) $(LDFLAGS) -o $@ $^

main.o : main.c $(INC)/parsetools.h $(INC)/builtins.h $(INC)/constants.h
	$(CC) $(CFLAGS) -c -o $@ $<

parsetools.o : $(SRC)/parsetools.c $(INC)/constants.h
	$(CC) $(CFLAGS) -c -o $@ $<


run : lobo_shell.x
	./lobo_shell.x

test : lobo_shell.x
	mkdir -p tests/integration/output
	for t in tests/integration/*.sh; do sh $$t; done

leak : lobo_shell.x
	leaks -atExit -- ./lobo_shell.x

clean :
	rm -f *.x *.o *.out *~
	rm -rf tests/integration/output