CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET1 = parent_env
TARGET2 = child_env
SRC1 = src/main.c
SRC2 = src/child_env.c

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

clean:
	rm -f $(TARGET1) $(TARGET2)
