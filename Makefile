CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
LDLIBS = -lncurses

TARGET = game_of_life
SOURCE = src/game_of_life.c

.PHONY: all clean rebuild format

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LDLIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

format:
	clang-format -i $(SOURCE)
