CC = cc
CFLAGS = -Wall -Wextra -O2

TARGETS = load_balancer backend client

all: $(TARGETS)

load_balancer: load_balancer.c
	$(CC) $(CFLAGS) -o $@ $^

backend: backend.c
	$(CC) $(CFLAGS) -o $@ $^

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

