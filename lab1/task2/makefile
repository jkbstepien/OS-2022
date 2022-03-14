#CC = gcc
#CFLAGS = -g -Wall
#
#all: static shared
#
#static:
#	$(CC) $(CFLAGS) -c library.c -o library.o
#	ar rcs liblibrary.a library.o
#	$(CC) $(CFLAGS) -o main main.c library.o
#
#shared: _shared
#	$(CC) $(CFLAGS) -o main main.c library.o
#
#_shared:
#	$(CC) $(CFLAGS) -fPIC -c library.c -o library.o
#	$(CC) $(CFLAGS) -shared -o liblibrary.so library.o
#
#dynamic: _shared
#	$(CC) main.c -o main -L. -ldl -D dynamic
#
#clean:
#	rm -rf *.o *.a *.so main report2.txt
#
#test1:
#	@./main createArray 4 wcFiles "small1.txt small2.txt" saveTmpIntoArray removeBlock 0

CC = gcc
CFLAGS = -Wall -std=c11

all: static reports clean

static:
	$(CC) $(CFLAGS) -c mylib.c
	ar rcs libmylib.a mylib.o
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o libmylib.a -o main

reports: static
	@printf "small_test\n" >> report2.txt
	@./main `cat ./resources/test_args1.txt | tr '\n' ' '` >> report2.txt

	@printf "medium_test\n" >> report2.txt
	@./main `cat ./resources/test_args2.txt | tr '\n' ' '` >> report2.txt

clean:
	rm -f main tmp report2.txt *.o *.a *.so