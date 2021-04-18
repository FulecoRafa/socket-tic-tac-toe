//
// Created by Gabriel Souza on 18/04/21.
//

#ifndef SOCKET_TIC_TAC_TOE_GAME_H
#define SOCKET_TIC_TAC_TOE_GAME_H
#define SIZE 3
#include <stdlib.h>

int **create_board();
void make_move(int* error, int **board, int i, int j, int value);


#endif //SOCKET_TIC_TAC_TOE_GAME_H
