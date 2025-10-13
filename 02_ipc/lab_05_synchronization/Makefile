TARGET = sync_demo
SRC = src/main.c

CFLAGS = -Wall -Wextra -O2 -pthread

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

