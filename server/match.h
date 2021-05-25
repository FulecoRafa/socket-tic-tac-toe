#ifndef SOCKET_TIC_TAC_TOE_MATCH_H
#define SOCKET_TIC_TAC_TOE_MATCH_H

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "../helpers/macros.h"
#include "../helpers/events.h"
#include <unistd.h>

_Noreturn void match_making(int server_socket, int max_players,
                            pthread_t *game_threads, int *current_threads);

typedef struct match {
  int players[2];
  pthread_t listeners[2];
  pthread_mutex_t mutex;
  int **board;
  int who_is_playing;
  bool is_running;
} match_t;


typedef struct listener {
  int connection;
  event_t *event;
  pthread_mutex_t *mutex;
} listener_t;

#endif //SOCKET_TIC_TAC_TOE_MATCH_H
