# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline -lncurses

# Object Files
OBJS = shell.o builtin.o

# Target rule
RPShell: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Individual object file rules
shell.o: shell.c builtin.h
	$(CC) $(CFLAGS) -c $< -o $@

builtin.o: builtin.c builtin.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and executable
clean:
	rm -f *.o RPShell

.PHONY: clean
