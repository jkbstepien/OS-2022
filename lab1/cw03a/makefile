CC=gcc
CFLAGS=-Wall -std=c11

all: test_all

static:
	$(CC) $(CFLAGS) -c mylib.c
	ar rcs lmylib.a mylib.o
	$(CC) $(CFLAGS) -o main main.c mylib.o

shared:
	$(CC) $(CFLAGS) -c -fPIC mylib.c
	$(CC) $(CFLAGS) -shared -fPIC -o mylib.so mylib.o
	$(CC) $(CFLAGS) -o main main.c mylib.o

dynamic:
	$(CC) $(CFLAGS) -c -fPIC mylib.c
	$(CC) $(CFLAGS) -shared -fPIC -o mylib.so mylib.o
	$(CC) $(CFLAGS) main.c -o main -L. -ldl -D DYNAMIC_LIBRARY

test_all: clean
	@printf "TESTS FOR STATIC LIBRARIES:\n" >> report3a.txt
	make static
	make reports

	@printf "TESTS FOR SHARED LIBRARIES:\n" >> report3a.txt
	make shared
	make reports

	@printf "TESTS FOR DYNAMIC LIBRARIES:\n" >> report3a.txt
	make dynamic
	make reports

reports:
	@printf "TEST_1:\n" >> report3a.txt

	@./main start_timer create_table 1 wc_files "./resources/small_test1 ./resources/small_test2" \
	load_block_to_table remove_block 0 end_timer >> report3a.txt

	@printf "\nTEST_2:\n" >> report3a.txt

	@./main start_timer create_table 2 wc_files "./resources/medium_test1" \
	load_block_to_table wc_files "./resources/medium_test1" load_block_to_table remove_block 1 end_timer >> report3a.txt

	@printf "\nTEST_3\n" >> report3a.txt

	@./main start_timer create_table 4 wc_files "./resources/big_test1 ./resources/big_test2 ./resources/big_test3" \
	load_block_to_table remove_block 0 wc_files "./resources/big_test1 ./resources/big_test1 ./resources/big_test1" \
	load_block_to_table wc_files "./resources/big_test1 ./resources/big_test2 ./resources/big_test3" \
	load_block_to_table remove_block 1 end_timer >> report3a.txt

	rm -f report2.txt

clean:
	rm -f tmp main report3a.txt report2.txt *.o *.a *.so