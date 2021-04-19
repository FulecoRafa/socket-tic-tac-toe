//
// Created by Gabriel Souza on 18/04/21.
//

#include "match.h"

void *listener(void *args) {
  client_listener_t *client_listener = (client_listener_t *) args;
  int size;
  char message[5];
  do {
    size = recv((int) client_listener->connection, message, 5, 0);
    message[size] = '\0';
    if (size > 0) {
      printf("Received: %d %s\n", size, message);
      pthread_mutex_lock(client_listener->mutex);
      strcpy(client_listener->event->message, message);
      client_listener->event->is_new = true;
      pthread_mutex_unlock(client_listener->mutex);
    }
  } while (size != -1);
  return NULL;
}

void *send_message(int client_connection, char *message) {
  int bytes_sent = send(client_connection, message, strlen(message), 0);
  printf("Sent %d %s\n", bytes_sent, message);
  free(message);
  return NULL;
}

client_match_t *create_client_match(int connection) {
  client_match_t *match = (client_match_t *) malloc(sizeof(client_match_t));
  match->is_running = true;
  match->board = create_board();
  match->can_play = false;
  match->connection = connection;
  match->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(match->mutex, NULL);
  return match;
}

client_listener_t *create_client_listener(int connection, client_match_t *match, event_t *event) {
  client_listener_t *client_listener = (client_listener_t *) malloc(sizeof(client_listener_t));
  client_listener->mutex = match->mutex;
  client_listener->event = event;
  client_listener->connection = connection;
  return client_listener;
}

void print_debug_board(int **board) {
  for (int i = 0; i < SIZE; i++) {
    printf("\n");
    for (int j = 0; j < SIZE; j++) {
      printf("%d ", board[i][j]);
    }
  }
  printf("\n");
}

char *encode_message(events_enum action, char pos_i, char pos_j) {
  char *message = (char *) malloc(5 * sizeof(char));
  message[0] = action;
  message[1] = pos_i;
  message[2] = pos_j;
  message[3] = '0';
  message[4] = '\0';
  return message;
}

move_t decode_message(event_t *event) {
  move_t move;
  move.pos_i = event->message[1] - '0';
  move.pos_j = event->message[2] - '0';
  move.value = event->message[3] == '2' ? -1 : 1;
  return move;
}

void handle_turn(client_match_t *match) {
  char pos_i, pos_j;
  printf("\n\n\nIt's your turn! \n");
  print_debug_board(match->board);
  scanf("%c %c", &pos_i, &pos_j);
  fflush(stdin);
  send_message(match->connection, encode_message(play, pos_i, pos_j));
}

void handle_update_board(client_match_t *match, event_t *event) {
  int error = 0;
  make_move(&error, match->board, decode_message(event));
}

void exec_game(client_match_t *match, event_t *event) {
  events_enum action = event->message[0];
  switch (action) {
    case finding_match:
      printf("Finding match... \n");
      return;
    case match_found:
      printf("Match found! \n");
      return;
    case your_turn:
      return handle_turn(match);
    case wait_turn:
      printf("Wait enemy turn \n");
      return;
    case update_board:
      return handle_update_board(match, event);
    case win:
      printf("You win! \n");
      return;
    case lose:
      printf("You lose \n");
      return;
    case draw:
      printf("Draw \n");
      return;

    default: {
      return;
    }
  }
}

_Noreturn void start_game(int connection) {
  event_t event;
  pthread_t listener_thread;
  client_match_t *match = create_client_match(connection);
  client_listener_t *client_listener = create_client_listener(connection, match, &event);
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&listener_thread, NULL, listener, client_listener);
  while (match->is_running) {
    if (event.is_new) {
      pthread_mutex_lock(match->mutex);
      event.is_new = false;
      exec_game(match, &event);
      pthread_mutex_unlock(match->mutex);
    }
  }
}