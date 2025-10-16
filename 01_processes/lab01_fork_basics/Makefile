CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET = fork_demo
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

