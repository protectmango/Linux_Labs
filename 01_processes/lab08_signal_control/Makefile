CC = cc
CFLAGS = -Wall -Wextra -O2
TARGET = signal_control
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

