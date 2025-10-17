CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET1 = parent_exec
TARGET2 = child_exec
SRC1 = src/main.c
SRC2 = src/child_exec.c

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

clean:
	rm -f $(TARGET1) $(TARGET2)

