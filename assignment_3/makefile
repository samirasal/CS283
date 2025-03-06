# Makefile for compiling demonstration C files

# Compiler
CC = gcc

# Standard compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SOURCES = fork-pipe.c fork-pipeline.c

# Executable names (remove .c extension for executables)
EXECUTABLES = $(SOURCES:.c=)

# Default target: build all executables
all: $(EXECUTABLES)

# Rule for standard compilation
$(EXECUTABLES): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean up compiled files
clean:
	rm -f $(EXECUTABLES) 

# Phony targets
.PHONY: all clean