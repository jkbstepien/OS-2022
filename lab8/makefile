CC = gcc
CFLAGS = -g -Wall

all:
	$(CC) $(CFLAGS) main.c -o main -pthread
	./main 1 blocks casablanca.ascii.pgm negated1.ascii.pgm
	./main 1 numbers casablanca.ascii.pgm negated2.ascii.pgm
	./main 2 blocks casablanca.ascii.pgm negated1.ascii.pgm
	./main 2 numbers casablanca.ascii.pgm negated2.ascii.pgm
	./main 3 blocks casablanca.ascii.pgm negated1.ascii.pgm
	./main 3 numbers casablanca.ascii.pgm negated2.ascii.pgm
	./main 4 blocks casablanca.ascii.pgm negated1.ascii.pgm
	./main 4 numbers casablanca.ascii.pgm negated2.ascii.pgm

clean:
	rm -f main Times.txt negated?.ascii.pgm