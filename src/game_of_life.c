#include <ncurses.h>
#include <stdio.h>

#define ROWS 25
#define COLUMNS 80
#define INITIAL_DELAY 200
#define MIN_DELAY 50
#define MAX_DELAY 1000
#define DELAY_STEP 50

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

static void close_terminal(SCREEN *screen, FILE *input, FILE *output) {
  if (screen != NULL) {
    endwin();
    delscreen(screen);
  }
  if (input != NULL) {
    fclose(input);
  }
  if (output != NULL) {
    fclose(output);
  }
}

static SCREEN *start_screen(FILE *input, FILE *output, int *status) {
  SCREEN *screen;
  int screen_rows;
  int screen_columns;

  screen = newterm(NULL, output, input);
  if (screen == NULL) {
    *status = -1;
    return NULL;
  }
  set_term(screen);
  getmaxyx(stdscr, screen_rows, screen_columns);
  if (screen_rows < ROWS || screen_columns < COLUMNS) {
    *status = 0;
    return screen;
  }
  if (cbreak() == ERR || noecho() == ERR || keypad(stdscr, TRUE) == ERR) {
    *status = -1;
    return screen;
  }

  curs_set(0);
  *status = 1;
  return screen;
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

static int count_neighbors(GameField field, int row, int column) {
  int count = 0;
  int row_offset;
  int column_offset;
  int neighbor_row;
  int neighbor_column;

  for (row_offset = -1; row_offset <= 1; row_offset++) {
    for (column_offset = -1; column_offset <= 1; column_offset++) {
      if (row_offset != 0 || column_offset != 0) {
        neighbor_row = (row + row_offset + ROWS) % ROWS;
        neighbor_column = (column + column_offset + COLUMNS) % COLUMNS;
        count += field[neighbor_row][neighbor_column];
      }
    }
  }
  return count;
}

static void calculate_next_field(GameField current, GameField next) {
  int row;
  int column;
  int neighbors;

  for (row = 0; row < ROWS; row++) {
    for (column = 0; column < COLUMNS; column++) {
      neighbors = count_neighbors(current, row, column);
      next[row][column] =
          neighbors == 3 || (current[row][column] && neighbors == 2);
    }
  }
}

static void copy_field(GameField source, GameField destination) {
  int row;
  int column;

  for (row = 0; row < ROWS; row++) {
    for (column = 0; column < COLUMNS; column++) {
      destination[row][column] = source[row][column];
    }
  }
}

static int change_delay(int delay, int key) {
  if ((key == 'a' || key == 'A') && delay > MIN_DELAY) {
    delay -= DELAY_STEP;
  } else if ((key == 'z' || key == 'Z') && delay < MAX_DELAY) {
    delay += DELAY_STEP;
  }
  return delay;
}

static void game_loop(GameField current) {
  GameField next;
  int delay = INITIAL_DELAY;
  int key;

  timeout(delay);
  display_field(current);
  while ((key = getch()) != ' ') {
    if (key == 'a' || key == 'A' || key == 'z' || key == 'Z') {
      delay = change_delay(delay, key);
      timeout(delay);
    } else if (key == ERR) {
      calculate_next_field(current, next);
      copy_field(next, current);
      display_field(current);
    }
  }
}

int main(void) {
  GameField field;
  FILE *control_input;
  FILE *control_output;
  SCREEN *screen;
  int screen_status;

  if (!read_field(field)) {
    fprintf(stderr,
            "Ошибка: поле должно содержать 25 строк по 80 символов 0 или 1.\n");
    return 1;
  }
  control_input = fopen("/dev/tty", "r");
  control_output = fopen("/dev/tty", "w");
  if (control_input == NULL || control_output == NULL) {
    fprintf(stderr, "Ошибка: не удалось открыть терминал для управления.\n");
    close_terminal(NULL, control_input, control_output);
    return 1;
  }

  screen = start_screen(control_input, control_output, &screen_status);
  if (screen_status == 0) {
    fprintf(stderr, "Ошибка: размер терминала должен быть не меньше 80x25.\n");
    close_terminal(screen, control_input, control_output);
    return 1;
  }
  if (screen_status < 0) {
    fprintf(stderr, "Ошибка: не удалось запустить ncurses.\n");
    close_terminal(screen, control_input, control_output);
    return 1;
  }

  game_loop(field);
  close_terminal(screen, control_input, control_output);
  return 0;
}
