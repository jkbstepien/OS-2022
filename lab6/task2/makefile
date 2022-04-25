CC = gcc
CFLAGS = -Wall -g

all:
	$(CC) $(CFLAGS) server.c -o server -lrt
	$(CC) $(CFLAGS) client.c -o client -lrt

clean:
	rm -f server client