#ifndef UI_H
#define UI_H

#include <ncurses.h>

// Start curses UI
void ui_init() {
  initscr();
  raw();
  noecho();
  timeout(50);
  keypad(stdscr, 1);
  curs_set(0);
  start_color();

  // Used colors
  init_pair(1, COLOR_BLACK, COLOR_CYAN);
}

// Close curses UI
void ui_close() {
  curs_set(1);
  endwin();
}

void ui_pause(int time){
    timeout(time);
    getch();
}

void draw_box(int x, int y, int c) {
    //char buf[10];
    //sprintf(buf, "%d", c);
    //addstr         (buf);
  if (c == 1) {
    addstr         (" ^ ");
    mvaddstr(y+1, x, "(o)");
    mvaddstr(y+2, x, " v ");
  } else if (c == -1) {
    addstr         ("\\ /");
    mvaddstr(y+1, x, " x ");
    mvaddstr(y+2, x, "/ \\");
  } else {
    addstr         ("   ");
    mvaddstr(y+1, x, "   ");
    mvaddstr(y+2, x, "   ");
  }
}

void draw_board(int **places, int curs_pos[2]) {
  int x = 2, y = 2;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (curs_pos[0] == i && curs_pos[1] == j)
        attron(COLOR_PAIR(1));
      move(y, x);
      draw_box(x, y, places[i][j]);
      attroff(COLOR_PAIR(1));
      if (j != 2) {
        mvaddstr(y    , x + 3, "|");
        mvaddstr(y + 1, x + 3, "|");
        mvaddstr(y + 2, x + 3, "|");
      }
      x += 4;

    }
    x -= 12;
    y += 4;
    if (i != 2) {
      mvaddstr(y - 1, x, "---+---+---");
      move(y, x - 12);
    }
  }
  refresh();
}

void draw_msg(const char* msg) {
  mvaddstr (14, 2, "\n");
  mvaddstr (14, 2, msg);
}

#endif