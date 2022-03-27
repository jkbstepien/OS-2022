CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

all:
	$(CC) $(CFLAGS) main.c -o main
	./main 6
	./main
	./main 2 3

clean:
	rm -f main *.o