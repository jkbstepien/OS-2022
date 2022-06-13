CC = gcc
CFLAGS = -Wall -ggdb3 -lpthread

all:
	$(CC) -o client client.c $(CFLAGS)
	$(CC) -o server server.c $(CFLAGS)

clean:
	rm -rf socket server client