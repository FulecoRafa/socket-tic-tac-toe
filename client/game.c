#include <ncurses.h>
#include "game.h"

/* Function allocates a tic-tac-toe board with 0 in every place */
int **create_board() {
  int **board = (int **) malloc(SIZE * sizeof(int *));
  for (int i = 0; i < SIZE; i++) board[i] = (int *) malloc(SIZE * sizeof(int));
  for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) board[i][j] = 0;
  return board;
}

/* Function unallocate board */
void destroy_board(int** board){
  for (int i = 0; i < SIZE; i++)
    free(board[i]);
  free(board);
}

/* Checks if player movement is valid, that is, if chosen place
 * was not yet played in. */
void make_move(int *error, int **board, move_t move) {
  if (board[move.pos_i][move.pos_j] != 0)
    *error = 1;
  board[move.pos_i][move.pos_j] = move.value;
}
