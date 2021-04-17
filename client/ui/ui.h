#ifndef _TICTACTOE_UI_
#define _TICTACTOE_UI_

void ui_init();
void ui_close();
void draw_board(char places[9], int curs_pos[2], int win[3]);
void draw_msg(const char* msg);

#endif
