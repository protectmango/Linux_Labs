CC = cc
CFLAGS = -Wall -Wextra -O2

TARGETS = load_balancer_kq backend client

all: $(TARGETS)

load_balancer_kq: load_balancer_kq.c
	$(CC) $(CFLAGS) -o $@ $^

backend: backend.c
	$(CC) $(CFLAGS) -o $@ $^

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

