//
// Created by Gabriel Souza on 18/04/21.
//

#ifndef SOCKET_TIC_TAC_TOE_GAME_H
#define SOCKET_TIC_TAC_TOE_GAME_H
#define SIZE 3
#include <stdlib.h>

typedef struct move {
  int pos_i;
  int pos_j;
  int value;
} move_t;

int **create_board();
void make_move(int* error, int **board, move_t move);

#endif //SOCKET_TIC_TAC_TOE_GAME_H
