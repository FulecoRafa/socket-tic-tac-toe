//
// Created by Gabriel Souza on 17/04/21.
//

#ifndef SOCKET_TIC_TAC_TOE_MATCH_H
#define SOCKET_TIC_TAC_TOE_MATCH_H

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../helpers/macros.h"

_Noreturn void match_making (int server_socket, int max_players, pthread_t *game_threads, int *current_threads);
//void create_match(int player_conn_1, int player_conn_2);

typedef struct match {
  int players[2];
  pthread_t listeners[2];
  pthread_t emitters[2];
  pthread_mutex_t *mutex;
  int **board;
  int who_is_playing;
  bool is_running;
} Match;

typedef struct event {
  bool is_new;
  char message[256];
  int sender;
} Event;

typedef struct listener {
  int connection;
  Event *event;
  pthread_mutex_t *mutex;
} Listener;

#endif //SOCKET_TIC_TAC_TOE_MATCH_H
