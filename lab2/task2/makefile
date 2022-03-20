CC = gcc
CFLAGS = -Wall -Wunused-variable -std=c11 -ggdb

lib:
	$(CC) $(FLAGS) main_lib.c -o main_lib
	./main_lib u test_file.txt

sys:
	$(CC) $(CFLAGS) main_sys -o main_sys
	./main_sys @ test_file.txt

test:
	$(CC) $(CFLAGS) test.c -o test
	./test

clean:
	rm -f main_lib main_sys test pomiar_zad_2.txt *.o

# CC = gcc
# CFLAGS = -Wall -g


# compile:
# 		$(CC) $(FLAGS) test.c -o test

# run:  compile
# 		./test a test_file.txt
# 		@echo " " >> pomiar_zad_2.txt
# 		@echo "for 'a' and 'test_file.txt" >> pomiar_zad_2.txt
# 		./test a test_file.txt >> pomiar_zad_2.txt

# clean:
# 		rm -f test