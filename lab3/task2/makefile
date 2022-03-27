CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

all:
	$(CC) $(CFLAGS) main.c -o main
	./main 0.000001 10000

clean:
	rm -f main test *.o ./results/w*.txt