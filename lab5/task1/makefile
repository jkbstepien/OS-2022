CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

all: clean
	$(CC) $(CFLAGS) main.c -o main
	./main cmd.txt

clean:
	rm -rf main test *.o