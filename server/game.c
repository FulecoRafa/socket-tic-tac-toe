#include "game.h"

#define SIZE 3

void make_move(int *error, int **board, move_t move) {
  printf("Trying to put move: %d %d\n", move.pos_i, move.pos_j);
  if (board[move.pos_i][move.pos_j] != 0) {
    *error = 1;
    return;
  }
  board[move.pos_i][move.pos_j] = move.value;
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

int check_draw(int **board) {
  int sum = 0;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board[i][j] ==0) sum++;
    }
  }
  return sum == 0;
}
