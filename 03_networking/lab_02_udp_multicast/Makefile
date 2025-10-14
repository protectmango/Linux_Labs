CFLAGS = -Wall -Wextra -O2

all: udp_server udp_client multicast_sender multicast_receiver

udp_server: src/udp_server.c
	$(CC) $(CFLAGS) -o $@ $^

udp_client: src/udp_client.c
	$(CC) $(CFLAGS) -o $@ $^

multicast_sender: src/multicast_sender.c
	$(CC) $(CFLAGS) -o $@ $^

multicast_receiver: src/multicast_receiver.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f udp_server udp_client multicast_sender multicast_receiver

