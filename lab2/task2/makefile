CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

lib:
	$(CC) $(FLAGS) main_lib.c -o main_lib
	./main_lib u jane_austen.txt
	# ./main_lib

sys:
	$(CC) $(CFLAGS) main_sys.c -o main_sys
	./main_sys u jane_austen.txt
	# ./main_sys

clean:
	rm -f main_lib main_sys test pomiar_zad_2.txt *.o
