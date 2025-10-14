CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET = daemon_logger

all: $(TARGET)

$(TARGET): daemon_logger.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

