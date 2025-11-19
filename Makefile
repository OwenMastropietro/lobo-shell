# Directories
SRC = src
INC = include
OBJ = obj
BIN = bin
UNIT_TEST_SRCS := $(wildcard tests/unit/*.c)
UNIT_TEST_BIN := bin/unit_tests.x
UNIT_OBJ_DIR := obj/unit
UNIT_TEST_OBJS := $(UNIT_TEST_SRCS:tests/unit/%.c=$(UNIT_OBJ_DIR)/%.o)

# Compiler
CFLAGS = -I$(INC) -Wall -Wextra -pedantic -g
LDFLAGS = 
CC = gcc
SHELL = bash

$(UNIT_OBJ_DIR):
	mkdir -p $(UNIT_OBJ_DIR)

bin:
	mkdir -p bin


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

leak : lobo_shell.x
	leaks -atExit -- ./lobo_shell.x

# Testing
$(UNIT_OBJ_DIR)/%.o : tests/unit/%.c | $(UNIT_OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(UNIT_TEST_BIN): $(UNIT_TEST_OBJS) parsetools.o builtins.o | bin
	$(CC) $(LDFLAGS) -o $@ $^

test-unit: $(UNIT_TEST_BIN)
	./$(UNIT_TEST_BIN)

test-integration : lobo_shell.x
	mkdir -p tests/integration/output
	for t in tests/integration/*.sh; do sh $$t; done

test: test-unit test-integration
	@echo "All tests completed."


clean :
	rm -f *.x *.o *.out *~
	rm -rf tests/integration/output
	rm -rf $(UNIT_OBJ_DIR)
	rm -f $(UNIT_TEST_BIN)
