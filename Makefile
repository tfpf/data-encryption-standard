SHELL  = /bin/sh
CC     = gcc
CFLAGS = -O2 -std=c11 -Wall -Wextra
RM     = rm -f

Sources    = des.c
Executable = des

.PHONY: clean

$(Executable): $(Sources)
	$(CC) $(CFLAGS) -o $(Executable) $(Sources)

clean:
	$(RM) $(Executable)
