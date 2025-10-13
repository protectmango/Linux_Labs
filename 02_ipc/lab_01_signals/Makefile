TARGET = signals_demo
SRC = src/main.c

CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

