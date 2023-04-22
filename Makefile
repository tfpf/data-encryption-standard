SHELL  = /bin/sh
CC     = gcc
CFLAGS = -O2 -std=c11 -Wall -Wextra -I./mersenne-twister/include
RM     = rm -f

Sources    = des.c ./mersenne-twister/lib/mt19937_64.c
Executable = des

.PHONY: clean

$(Executable): $(Sources)
	$(CC) $(CFLAGS) -o $(Executable) $(Sources)

clean:
	$(RM) $(Executable)
