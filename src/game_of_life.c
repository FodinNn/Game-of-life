#include <ncurses.h>
#include <stdio.h>

#define ROWS 25
#define COLUMNS 80

typedef int GameField[ROWS][COLUMNS];

static int read_row(int row[COLUMNS]) {
  int column;
  int character;

  for (column = 0; column < COLUMNS; column++) {
    character = getchar();
    if (character != '0' && character != '1') {
      return 0;
    }
    row[column] = character - '0';
  }

  character = getchar();
  if (character == '\r') {
    character = getchar();
    if (character != '\n') {
      return 0;
    }
  }

  return character == '\n' || character == EOF;
}

static int read_field(GameField field) {
  int row;
  int character;

  for (row = 0; row < ROWS; row++) {
    if (!read_row(field[row])) {
      return 0;
    }
  }

  character = getchar();
  return character == EOF;
}

static int start_screen(void) {
  if (initscr() == NULL) {
    return 0;
  }

  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  return 1;
}

static void display_field(GameField field) {
  int row;
  int column;

  erase();
  for (row = 0; row < ROWS; row++) {
    for (column = 0; column < COLUMNS; column++) {
      mvaddch(row, column, field[row][column] ? 'O' : ' ');
    }
  }
  refresh();
}

int main(void) {
  GameField field;

  if (!read_field(field) || freopen("/dev/tty", "r", stdin) == NULL) {
    return 1;
  }
  if (!start_screen()) {
    return 1;
  }

  display_field(field);
  getch();
  endwin();
  return 0;
}
