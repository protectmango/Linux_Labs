CC = cc
CFLAGS = -Wall -Wextra -O2
TARGET = redir_shell
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

