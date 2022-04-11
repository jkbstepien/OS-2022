CC = gcc
CFLAGS = -g -Wall

all: clean main
	./main date
	./main sender
	./main "kubek0108@gmail.com" "test_message" "XD"

main:
	$(CC) $(FLAGS) main.c -o main

clean:
	rm -f main