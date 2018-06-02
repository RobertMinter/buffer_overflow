CC = gcc -std=c99 -Wall -Wextra -Werror -lm

all: buffer_overflow.x

buffer_overflow.x: buffer_overflow.c
	$(CC) -o buffer_overflow.x buffer_overflow.c

clean:
	rm *.x
