//
// Created by Gabriel Souza on 17/04/21.
//

#include "match.h"

void *listen_client(void *args) {
  Listener *listener = (Listener *) args;
  int size;
  char message[260];
  do {
    size = recv((int) listener->connection, message, 256, 0);
    message[size] = '\0';
    if (size > 1) {
      pthread_mutex_lock(listener->mutex);
      strcpy(listener->event->message, message);
      listener->event->is_new = true;
      listener->event->sender = listener->connection;
      pthread_mutex_unlock(listener->mutex);
    }
  } while (size != -1);
  return NULL;
}

void *send_message(int client_connection, char* message) {
  send(client_connection, message, strlen(message), 0);
  return NULL;
}

Listener *create_listener(int connection, Event* event, pthread_mutex_t *mutex) {
  Listener *new_listener = (Listener *)malloc(sizeof(Listener));
  new_listener->mutex = mutex;
  new_listener->connection = connection;
  new_listener->event = event;
  return new_listener;
}

void *start_game(void * args) {
  Match *match = (Match*) args;
  Event event;
  Listener *player_1_listener = create_listener(match->players[0], &event, match->mutex);
  Listener *player_2_listener = create_listener(match->players[1], &event, match->mutex);
  printf("Starting game with %d and %d\n", match->players[0], match->players[1]);
  pthread_create(&match->listeners[0], NULL, listen_client, (void *) player_1_listener);
  pthread_create(&match->listeners[1], NULL, listen_client, (void *) player_2_listener);
  while (match->is_running) {
    if (event.is_new) {
      printf("Sender: %d, Message: %s \n", event.sender, event.message);
      pthread_mutex_lock(match->mutex);
      event.is_new = false;
      pthread_mutex_unlock(match->mutex);
      send_message(event.sender, "Server reply");
    }
  }
  return NULL;
}

Match *create_match(int player1, int player2) {
  Match *new_match = (Match*) malloc(sizeof(Match));
  new_match->players[0] = player1;
  new_match->players[1] = player2;
  new_match->board = NULL;
  new_match->mutex = (pthread_mutex_t *) malloc(sizeof (pthread_mutex_t));
  new_match->is_running = true;
  new_match->who_is_playing = -1;
  pthread_mutex_init(new_match->mutex, NULL);
  return new_match;
}

_Noreturn void match_making (int server_socket, int max_players, pthread_t *game_threads, int *current_threads) {
  int conns[max_players], players_connected = 0;
  while (true) {
    if (players_connected == max_players - 1) continue;
    // Player connection
    int accepted_connection = accept(server_socket, 0, 0);

    if (accepted_connection == -1) eprintf("Someone tried to connect, but it went wrong...\n");

    conns[players_connected] = accepted_connection;
    printf("Client %d has just entered queue\n", conns[players_connected]);
    players_connected++;

    if (players_connected % 2 == 0) {
      Match *match = create_match(conns[players_connected - 1], conns[players_connected - 2]);
      pthread_create(&game_threads[*current_threads], NULL, start_game, (void *) match);
      *current_threads = *current_threads + 1;
    }
  }
}
