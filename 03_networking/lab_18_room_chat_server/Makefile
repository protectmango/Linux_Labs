CC=gcc
CFLAGS=-Wall -Wextra -O2 -pthread

all: server client

server: src/server.c
	$(CC) $(CFLAGS) -o server src/server.c

client: src/client.c
	$(CC) $(CFLAGS) -o client src/client.c

clean:
	rm -f server client

