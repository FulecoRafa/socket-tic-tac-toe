#ifndef SOCKET_TIC_TAC_TOE_MATCH_H
#define SOCKET_TIC_TAC_TOE_MATCH_H

#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include<unistd.h>
#include "../helpers/macros.h"
#include "../helpers/events.h"
#include "game.h"

typedef struct client_match {
  int **board;
  int connection;
  bool can_play;
  bool is_running;
  struct {
    bool valid;
    char row;
    char col;
  } play_intent;
  pthread_mutex_t *mutex;
} client_match_t;

typedef struct client_listener {
  int connection;
  event_t *event;
  pthread_mutex_t *mutex;
} client_listener_t;

void start_game (int connection);

#endif //SOCKET_TIC_TAC_TOE_MATCH_H
