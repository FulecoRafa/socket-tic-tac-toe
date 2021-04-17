#include "ui.h"
#include <ncurses.h>

#define MAX(x,y) x>y?x:y
#define MIN(x,y) x>y?y:x

int main (void) {
  char places[9] = {
    'x', 'o', '.',
    '.', '.', '.',
    '.', '.', '.'
  };
  int cursor[2] = { 1, 1 };
  int win[6] = {
    0,2,
    1,1,
    2,0
  };
  ui_init();
  while (1) {
    draw_board(places, cursor, win);
    char c = getch();
    switch (c) {
    case 'a':
      cursor[1] = MAX(cursor[1] - 1, 0);
      break;
    case 'd':
      cursor[1] = MIN(cursor[1] + 1, 2);
      break;
    case 'w':
      cursor[0] = MAX(cursor[0] - 1, 0);
      break;
    case 's':
      cursor[0] = MIN(cursor[0] + 1, 2);
      break;
    case ' ':
      places[3 * cursor[0] + cursor[1]] = 'x';
      break;
    case 'm':
      draw_msg("This is a message");
      break;
    case 'e':
      draw_msg("This is an error");
      break;

    default:
      ui_close();
      return 0;
      break;
    }
  }
  return 0;
}
