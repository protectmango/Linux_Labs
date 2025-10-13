SERVER = server
CLIENT = client

CFLAGS = -Wall -Wextra -O2

all: $(SERVER) $(CLIENT)

$(SERVER): src/server.c
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT): src/client.c
	$(CC) $(CFLAGS) -o $@ $^

run-server: $(SERVER)
	./$(SERVER)

run-client: $(CLIENT)
	./$(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)

