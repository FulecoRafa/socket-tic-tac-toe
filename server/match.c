//
// Created by Gabriel Souza on 17/04/21.
//

#include "match.h"

char *encode_message(events_enum action, move_t move) {
  char *message = (char *) malloc(5 * sizeof(char));
  message[0] = action;
  message[1] = move.pos_i + '0';
  message[2] = move.pos_j + '0';
  message[3] = move.value == -1 ? '2' : '1'; // its easier to parse positive numbers
  message[4] = '\0';
  return message;
}

int get_move_value_by_sender(match_t *match, int sender) {
  if (match->players[0] == sender) return -1;
  return 1;
}


move_t decode_message(match_t *match, event_t *event) {
  move_t move;
  move.pos_i = event->message[1] - '0';
  move.pos_j = event->message[2] - '0';
  move.value = get_move_value_by_sender(match, event->sender);
  return move;
}

void delay(int time_in_millis) {
  clock_t start_time = clock();
  while (clock() < start_time + time_in_millis);
}

void *listen_client(void *args) {
  listener_t *listener = (listener_t *) args;
  int size;
  char message[5];
  do {
    size = recv((int) listener->connection, message, 5, 0);
    message[size] = '\0';
    if (size > 0) {
      pthread_mutex_lock(listener->mutex);
      strcpy(listener->event->message, message);
      listener->event->is_new = true;
      listener->event->sender = listener->connection;
      pthread_mutex_unlock(listener->mutex);
    }
  } while (size != -1);
  return NULL;
}

void *send_message(int client_connection, char *message) {
  delay(50); // @TODO remove this after fix message length on recv
  send(client_connection, message, strlen(message), 0);
  return NULL;
}

listener_t *create_listener(int connection, event_t *event, pthread_mutex_t *mutex) {
  listener_t *new_listener = (listener_t *) malloc(sizeof(listener_t));
  new_listener->mutex = mutex;
  new_listener->connection = connection;
  new_listener->event = event;
  return new_listener;
}

move_t create_empty_move() {
  move_t move;
  move.pos_i = 0;
  move.pos_j = 0;
  move.value = 0;
  return move;
}

move_t create_move_from_value(int value) {
  move_t move;
  move.pos_i = 0;
  move.pos_j = 0;
  move.value = value;
  printf("%d \n", value);
  return move;
}

int get_opposite_player_by_sender(match_t *match, int sender) {
  if (match->players[0] == sender) return match->players[1];
  return match->players[0];
}

void handle_play(match_t *match, event_t *event) {
  int error = 0;
  move_t decoded_message = decode_message(match, event);
  make_move(&error, match->board, decoded_message);
  if (error) {
    send_message(event->sender, encode_message(invalid_move, create_empty_move()));
    return;
  }
  send_message(match->players[0], encode_message(update_board, decoded_message)); // @TODO pass update data
  send_message(match->players[1], encode_message(update_board, decoded_message));
  int possible_winner = check_winner(match->board);
  if (possible_winner) {
    int winner = possible_winner == -1 ? 0 : 1;
    send_message(match->players[winner], encode_message(win, create_empty_move()));
    send_message(get_opposite_player_by_sender(match, match->players[winner]),
                 encode_message(lose, create_empty_move()));
    return;
  }
  if (check_draw(match->board)) {
    send_message(match->players[0], encode_message(draw, create_empty_move()));
    send_message(match->players[1], encode_message(draw, create_empty_move()));
    return;
  }
  send_message(event->sender, encode_message(wait_turn, create_empty_move()));
  send_message(get_opposite_player_by_sender(match, event->sender),
               encode_message(your_turn, create_move_from_value(get_move_value_by_sender(match, event->sender))));
}

void exec_game(match_t *match, event_t *event) {
  events_enum action = event->message[0];
  switch (action) {
    case play:
      return handle_play(match, event);
    default: {
      return;
    }
  }
}

void *start_game(void *args) {
  match_t *match = (match_t *) args;
  event_t event;
  listener_t *player_1_listener = create_listener(match->players[0], &event, match->mutex);
  listener_t *player_2_listener = create_listener(match->players[1], &event, match->mutex);
  printf("Starting game with %d and %d\n", match->players[0], match->players[1]);
  pthread_create(&match->listeners[0], NULL, listen_client, (void *) player_1_listener);
  pthread_create(&match->listeners[1], NULL, listen_client, (void *) player_2_listener);
  send_message(match->players[0], encode_message(match_found, create_empty_move()));
  send_message(match->players[1], encode_message(match_found, create_empty_move()));
  send_message(match->players[0], encode_message(wait_turn, create_empty_move()));
  send_message(match->players[1],
               encode_message(your_turn, create_move_from_value(get_move_value_by_sender(match, match->players[0]))));
  while (match->is_running) {
    if (event.is_new) {
      printf("Sender: %d, Message: %s \n", event.sender, event.message);
      pthread_mutex_lock(match->mutex);
      exec_game(match, &event);
      event.is_new = false;
      pthread_mutex_unlock(match->mutex);
    }
  }
  return NULL;
}

match_t *create_match(int player1, int player2) {
  match_t *new_match = (match_t *) malloc(sizeof(match_t));
  new_match->players[0] = player1;
  new_match->players[1] = player2;
  new_match->board = create_board();
  new_match->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  new_match->is_running = true;
  new_match->who_is_playing = -1;
  pthread_mutex_init(new_match->mutex, NULL);
  return new_match;
}

_Noreturn void match_making(int server_socket, int max_players, pthread_t *game_threads, int *current_threads) {
  int conns[max_players], players_connected = 0;
  while (true) {
    if (players_connected == max_players - 1) continue;
    // Player connection
    int accepted_connection = accept(server_socket, 0, 0);

    if (accepted_connection == -1) eprintf("Someone tried to connect, but it went wrong...\n");

    conns[players_connected] = accepted_connection;
    printf("Client %d has just entered queue\n", conns[players_connected]);
    players_connected++;
    send_message(accepted_connection, encode_message(finding_match, create_empty_move()));

    if (players_connected % 2 == 0) {
      match_t *match = create_match(conns[players_connected - 1], conns[players_connected - 2]);
      pthread_create(&game_threads[*current_threads], NULL, start_game, (void *) match);
      *current_threads = *current_threads + 1;
    }
  }
}
