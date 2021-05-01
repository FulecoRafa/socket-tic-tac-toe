//
// Created by Gabriel Souza on 18/04/21.
//

#include <ncurses.h>
//#include "ui.h"
#include "game.h"

int **create_board() {
  int **board = (int **) malloc(SIZE * sizeof(int *));
  for (int i = 0; i < SIZE; i++) board[i] = (int *) malloc(SIZE * sizeof(int));
  for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) board[i][j] = 0;
  return board;
}

void make_move(int *error, int **board, move_t move) {
  if (board[move.pos_i][move.pos_j] != 0) *error = 1;
  board[move.pos_i][move.pos_j] = move.value;
}
