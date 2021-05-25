#ifndef SOCKET_TIC_TAC_TOE_EVENTS_H
#define SOCKET_TIC_TAC_TOE_EVENTS_H

#define PAYLOAD_SIZE 4

#include "macros.h"

/* Game events for flow control */
typedef enum events {
  finding_match = 'F',
  match_found = 'M',
  wait_turn = 'W',
  your_turn = 'Y',
  play = 'P',
  win = 'J',
  lose = 'L',
  draw = 'D',
  quit = 'Q',
  restart = 'R',
  invalid_move = 'V',
  update_board = 'U',
} events_enum;

typedef struct event {
  bool is_new;
  char message[5];
  int sender;
} event_t;


#endif //SOCKET_TIC_TAC_TOE_EVENTS_H
