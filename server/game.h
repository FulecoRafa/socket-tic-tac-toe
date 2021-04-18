//
// Created by Gabriel Souza on 17/04/21.
//

#ifndef SOCKET_TIC_TAC_TOE_GAME_H
#define SOCKET_TIC_TAC_TOE_GAME_H
#include "stdlib.h"

int **create_board();
void make_move(int* error, int **board, int i, int j, int value);
int check_winner(int **board);
int check_draw(int **board);
#endif //SOCKET_TIC_TAC_TOE_GAME_H
