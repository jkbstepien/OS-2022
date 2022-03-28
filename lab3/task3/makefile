CC = gcc
CFLAGS = -Wall -Wunused-variable -std=gnu99 -ggdb

all:
	$(CC) $(CFLAGS) main.c -o main

	@echo "----------------------------------------Test for depth 1."
	./main ./test_dir sysopki 1
	@echo "----------------------------------------Test for depth 2."
	./main ./test_dir sysopki 2

clean:
	rm -rf main test *.o 