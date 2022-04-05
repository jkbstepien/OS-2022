CC = gcc
CFLAGS = -Wall -Wno-unused-variable

all:
	$(CC) $(CFLAGS) main.c -o main
	$(CC) $(CFLAGS) test_child.c -o test_child

	./main ignore fork
	./main handler fork
	./main mask fork
	./main pending fork

	./main ignore exec
	./main mask exec
	./main pending exec

clean:
	rm -f main test_child *.o