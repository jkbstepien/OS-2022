CC = gcc
CFLAGS = -Wall -std=c11 -ggdb

all: static shared clean

static:
	$(CC) $(CFLAGS) -c mylib.c
	ar rcs lmylib.a mylib.o

shared:
	$(CC) $(CFLAGS) -c -fPIC mylib.c
	$(CC) $(CFLAGS) -shared -fPIC -o mylib.so mylib.o

clean:
	rm -f main tmp *.o *.a *.so
