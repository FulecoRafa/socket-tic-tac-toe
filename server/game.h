#ifndef SOCKET_TIC_TAC_TOE_GAME_H
#define SOCKET_TIC_TAC_TOE_GAME_H
#include <stdlib.h>
#include <stdio.h>

typedef struct move {
  int pos_i;
  int pos_j;
  int value;
} move_t;

int **create_board();
void destroy_board(int **board);
void make_move(int *error, int **board, move_t move);
int check_winner(int **board);
int check_draw(int **board);

#endif //SOCKET_TIC_TAC_TOE_GAME_H
