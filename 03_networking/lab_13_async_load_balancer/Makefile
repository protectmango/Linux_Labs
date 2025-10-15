CC=gcc
CFLAGS=-Wall -Wextra -O2

all: backend load_balancer client

backend: src/backend_server.c
	$(CC) $(CFLAGS) -o backend src/backend_server.c

load_balancer: src/load_balancer.c
	$(CC) $(CFLAGS) -o load_balancer src/load_balancer.c

client: src/client.c
	$(CC) $(CFLAGS) -o client src/client.c

clean:
	rm -f backend load_balancer client

