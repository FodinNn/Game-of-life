#include <ncurses.h>

#define ROWS 25
#define COLUMNS 80

typedef int GameField[ROWS][COLUMNS];

static void fill_field_with_dead_cells(GameField field) {
  int row;
  int column;

  for (row = 0; row < ROWS; row++) {
    for (column = 0; column < COLUMNS; column++) {
      field[row][column] = 0;
    }
  }
}

int main(void) {
  GameField field;

  fill_field_with_dead_cells(field);

  return 0;
}
