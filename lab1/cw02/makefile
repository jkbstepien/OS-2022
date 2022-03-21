CC = gcc
CFLAGS = -Wall -std=c11

all: static reports

static:
	$(CC) $(CFLAGS) -c mylib.c
	ar rcs libmylib.a mylib.o
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o libmylib.a -o main

shared:
	$(CC) $(CFLAGS) -c -fPIC mylib.c
	$(CC) $(CFLAGS) -shared -fPIC -o mylib.so mylib.o

reports: static
	@printf "TASK 2:\n"
	@printf "TEST_1:\n" >> report2.txt

	@./main start_timer create_table 1 wc_files "./resources/small_test1 ./resources/small_test2" \
	load_block_to_table remove_block 0 end_timer

	@printf "\nTEST_2:\n" >> report2.txt

	@./main start_timer create_table 2 wc_files "./resources/medium_test1" \
	load_block_to_table wc_files "./resources/medium_test1" load_block_to_table remove_block 1 end_timer

	@printf "\nTEST_3\n" >> report2.txt

	@./main start_timer create_table 4 wc_files "./resources/big_test1 ./resources/big_test2 ./resources/big_test3" \
	load_block_to_table remove_block 0 wc_files "./resources/big_test1 ./resources/big_test1 ./resources/big_test1" \
	load_block_to_table wc_files "./resources/big_test1 ./resources/big_test2 ./resources/big_test3" \
	load_block_to_table remove_block 1 end_timer

clean:
	rm -f main tmp report2.txt *.o *.a *.so