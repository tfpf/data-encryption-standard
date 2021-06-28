CC = gcc
CFLAGS = -O0 -Wall -Wextra #-Werror

all: des.o

des.o: des.c
	$(CC) $(CFLAGS)  $^ -o $@
		
run: 
	./des.o
	
clean:
	rm -rf *.o
