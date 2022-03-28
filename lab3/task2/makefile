CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

all:
	mkdir results
	$(CC) $(CFLAGS) main.c -o main
	./main 0.000001 17000

clean:
	rm -Rf main *.o ./results/w*.txt ./results