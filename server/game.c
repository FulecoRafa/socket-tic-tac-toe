//
// Created by Gabriel Souza on 17/04/21.
//

#include "game.h"

#define SIZE 3

int **create_board() {
  int **board = (int **) malloc(SIZE * sizeof(int *));
  for (int i = 0; i < SIZE; i++) board[i] = (int *) malloc(SIZE * sizeof(int));
  for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) board[i][j] = 0;
  return board;
}

void make_move(int *error, int **board, int i, int j, int value) {
  if (board[i][j] != 0) *error = 1;
  board[i][j] = value;
}

int check_winner(int ** board) {
  int sum_row, sum_column, sum_diag = 0, sum_diag_inv = 0;
  for (int i = 0; i < SIZE; i ++) {
    sum_row = 0;
    sum_column = 0;
    for (int j = 0; j < SIZE; j++) {
      sum_row += board[i][j];
      sum_column += board[j][i];
    }
    sum_diag += board[i][i];
    sum_diag_inv += board[i][SIZE - i - 1];
    if (sum_row == 3 || sum_column == 3 || sum_diag == 3 || sum_diag_inv == 3) return 1;
    if (sum_row == -3 || sum_column == -3 || sum_diag == -3 || sum_diag_inv == -3) return -1;
  }
  return 0;
}