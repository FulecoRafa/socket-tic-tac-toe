#include "match.h"
#include "../helpers/message.h"

/* Takes event human-readable values and converts into encoded message
 * for transmission */
message_t encode_message (events_enum action, move_t move) {
  message_t message;

  message.content[0] = action;
  message.content[1] = move.pos_i + '0';
  message.content[2] = move.pos_j + '0';
  message.content[3] = move.value == -1 ? '2' : '1'; // its easier to parse positive numbers
  message.content[4] = '\0';

  return message;
}

/* Determines which player has sent the move */
int get_move_value_by_sender (match_t *match, int sender) {
  return match->players[0] == sender ? -1 : 1;
}

/* Takes an encoded message and returns human readable values */
move_t decode_message (match_t *match, event_t *event) {
  move_t move;

  move.pos_i = event->message[1] - '0';
  move.pos_j = event->message[2] - '0';
  move.value = get_move_value_by_sender(match, event->sender);

  return move;
}

/* Function listens for client messages and ready them for
 * management on other threads */
void listen_client (void *args) {
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

  return;
}

/* Sends encoded message to client */
void send_message (int client_connection, message_t message) {
  usleep(50000); // TODO check if this line is actually necessary
  send(client_connection, message.content, strlen(message.content), 0);
}

/* Generates a new listener for a client, with the structures needed
 * for synchronization between threads */
listener_t create_listener (int connection, event_t* event, pthread_mutex_t* mutex) {
  listener_t new_listener = {
    .mutex = mutex,
    .connection = connection,
    .event = event
  };
  return new_listener;
}

move_t create_empty_move () {
  move_t move = {
    .pos_i = 0,
    .pos_j = 0,
    .value = 0
  };
  return move;
}

move_t create_move_from_value (int value) {
  move_t move;
  move.pos_i = 0;
  move.pos_j = 0;
  move.value = value;
  return move;
}

int get_opposite_player_by_sender (match_t *match, int sender) {
  if (match->players[0] == sender) return match->players[1];
  return match->players[0];
}

/* Try to execute a players move and check if valid. Sends updated board
 * to both clients. If there is a winner, here it also resolves winners,
 * losers and draws */
void handle_play (match_t *match, event_t *event) {
  int error = 0;
  move_t decoded_message = decode_message(match, event);
  make_move(&error, match->board, decoded_message);

  // Invalid move
  if (error) {
    send_message(event->sender, encode_message(invalid_move, create_empty_move()));
    return;
  }

  // Update both players' boards
  send_message(match->players[0], encode_message(update_board, decoded_message));
  send_message(match->players[1], encode_message(update_board, decoded_message));

  // Check for winners
  int possible_winner = check_winner(match->board);
  if (possible_winner) {
    int winner = possible_winner == -1 ? 0 : 1;
    send_message(match->players[winner], encode_message(win, create_empty_move()));
    send_message(get_opposite_player_by_sender(match, match->players[winner]),
                 encode_message(lose, create_empty_move()));
    return;
  }

  // Check draw
  if (check_draw(match->board)) {
    send_message(match->players[0], encode_message(draw, create_empty_move()));
    send_message(match->players[1], encode_message(draw, create_empty_move()));
    return;
  }

  // Send turn messages
  send_message(event->sender, encode_message(wait_turn, create_empty_move()));
  send_message(
    get_opposite_player_by_sender(match, event->sender),
    encode_message(your_turn,
      create_move_from_value(
        get_move_value_by_sender(match, event->sender)
      )
    )
  );
}

/* Ends games in a match and exists gracefully */
void end_game (match_t *match){
  // Dealocate board
  destroy_board(match->board);

  // Close connections between players and server
  close(match->players[0]);
  close(match->players[1]);

  // Cancel match threads
  pthread_cancel(match->listeners[0]);
  pthread_cancel(match->listeners[1]);

  // Dealocate match
  free(match);
}

/* Deals with every message received from clients. If 'play',
 * delegate to `handle_play`*/
void exec_game (match_t *match, event_t *event) {
  if (event->message[0] == play){ // There has been a move
    handle_play(match, event);
  } else if (event->message[0] == quit) { // One player quitted
    printf("Match %p closing...\n", match);

    // Send message to opposite player that its oponnent left the game
    move_t decoded_message = decode_message(match, event);
    send_message(get_opposite_player_by_sender(match, event->sender),
                 encode_message(quit, decoded_message));

    // Exit gracefully
    end_game(match);
  }
}

/* Handles a game found and deals with starting a game in a new thread.
 * Also starts listeners threads */
void *start_game (void *args) {
  match_t *match = (match_t *) args;
  event_t event;

  // Open message listeners on threads
  listener_t player_listeners[2];
  player_listeners[0] = create_listener(match->players[1], &event, &match->mutex);
  player_listeners[1] = create_listener(match->players[0], &event, &match->mutex);
  printf("Starting game with %d and %d\n", match->players[0], match->players[1]);
  pthread_create(&match->listeners[0], NULL, (void *)listen_client,
                 (void *)&player_listeners[0]);
  pthread_create(&match->listeners[1], NULL, (void *)listen_client,
                 (void *)&player_listeners[1]);

  // Warn client that match was found and start game
  send_message(match->players[0], encode_message(match_found, create_empty_move()));
  send_message(match->players[1], encode_message(match_found, create_empty_move()));
  send_message(match->players[0], encode_message(wait_turn, create_empty_move()));
  send_message(
    match->players[1],
    encode_message(
      your_turn,
      create_move_from_value(
        get_move_value_by_sender(match, match->players[0])
      )
    )
  );

  // Loop while game is running. Keep thread alive. Deal with plays.
  while (match->is_running) {
    if (event.is_new) {
      printf("Sender: %d, Message: %s \n", event.sender, event.message);
      pthread_mutex_lock(&match->mutex);
      exec_game(match, &event);
      event.is_new = false;
      pthread_mutex_unlock(&match->mutex);
    }
  }

  // Exit gracefully
  end_game(match);
}

/* Alocates and returns a clean match */
match_t *create_match (int player1, int player2) {
  match_t *new_match = (match_t *) calloc(1, sizeof(match_t));

  // Default values
  new_match->players[0] = player1;
  new_match->players[1] = player2;
  new_match->board = create_board();
  new_match->is_running = true;
  new_match->who_is_playing = -1;
  pthread_mutex_init(&new_match->mutex, NULL);

  return new_match;
}

/* Starts a match making process, at every 2 players
 * start a match */
_Noreturn void match_making (
    int server_socket,
    int max_players,
    pthread_t* game_threads,
    int* current_threads
  ) {
  int conns[max_players], players_connected = 0;
  while (true) {
    // Ignore connections while max player
    // base has been reached
    if (players_connected == max_players - 1) {
      sleep(3);
      continue;
    }

    // Player connection
    int accepted_connection = accept(server_socket, 0, 0);

    if (accepted_connection == -1)
      eprintf("Someone tried to connect, but it went wrong...\n");

    //Register client in queue and send message for it to wait
    conns[players_connected] = accepted_connection;
    printf("Client %d has just entered queue\n", conns[players_connected]);
    players_connected++;
    send_message(accepted_connection, encode_message(finding_match, create_empty_move()));

    // When there are enough players to play a match, start
    if (players_connected % 2 == 0) {
      match_t *match = create_match(conns[players_connected - 1], conns[players_connected - 2]);
      pthread_create(&game_threads[*current_threads], NULL, start_game, (void *) match);
      (*current_threads)++;
    }
  }
}
