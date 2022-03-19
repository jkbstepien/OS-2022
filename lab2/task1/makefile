CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

all:
	make lib
	make sys

lib:
	$(CC) $(CFLAGS) main_lib.c -o main_lib
	@./main_lib ./file1 ./file2

sys:
	$(CC) $(CFLAGS) main_sys.c -o main_sys
	@./main_sys ./file1 ./file2

clean:
	rm -f main_lib main_sys ./file2 ./pomiar_zad_1.txt *.o