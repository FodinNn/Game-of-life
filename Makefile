CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
LDLIBS = -lncurses

TARGET = game_of_life
SOURCE = src/game_of_life.c

.PHONY: all clean rebuild format check-format cppcheck

FORMAT_FLAGS = --dry-run --Werror
CPPCHECK_FLAGS = --enable=all --suppress=missingIncludeSystem

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LDLIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

format:
	clang-format -i $(SOURCE)

check-format:
	clang-format $(FORMAT_FLAGS) $(SOURCE)

cppcheck:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck $(CPPCHECK_FLAGS) $(SOURCE); \
	else \
		echo "Ошибка: cppcheck не установлен." >&2; \
		exit 127; \
	fi
