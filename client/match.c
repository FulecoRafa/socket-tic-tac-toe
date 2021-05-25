#include "ui.h"
#include <ncurses.h>
#include "match.h"
#include "../helpers/message.h"

void delay (int time_in_millis) {
  clock_t start_time = clock();
  while (clock() < start_time + time_in_millis);
}

/* Function listens for server commands sent to the client and
 * parses them properly for usage in game threads */
void listener (void *args) { 
  client_listener_t *client_listener = (client_listener_t *) args;
  int size;
  char message[5];

  /* Listener loop */
  do {
    pthread_mutex_lock(client_listener->mutex);

    if (client_listener->event->is_new == false) {
      size = recv((int)client_listener->connection, message, 5, 0);
      message[size] = '\0';
      if (size > 0) {
        strcpy(client_listener->event->message, message);
        client_listener->event->is_new = true;
      }
    }

    pthread_mutex_unlock(client_listener->mutex);
  } while (size != -1);
}

/* Gets a parsed message and returns if there was an error sending bytes */
bool send_message (int client_connection, char *message) {
  int bytes_sent = send(client_connection, message, strlen(message), 0);
  bool error = bytes_sent == -1;

  return error;
}

/* Allocates and returns a fully set match with default values */
client_match_t *create_client_match (int connection) {
  client_match_t *match = (client_match_t *) calloc(1, sizeof(client_match_t));

  // Default values
  match->is_running = true;
  match->board = create_board();
  match->can_play = false;
  match->connection = connection;
  match->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(match->mutex, NULL);

  return match;
}

/* Allocates and returns a full client listener for checking events passed around
 * threads. */
client_listener_t *create_client_listener (int connection, client_match_t *match, event_t *event) {
  client_listener_t *client_listener = (client_listener_t *) malloc(sizeof(client_listener_t));

  // Default values
  client_listener->mutex = match->mutex;
  client_listener->event = event;
  client_listener->connection = connection;

  return client_listener;
}

/* [DEBUG] Prints board with simple printing for checking of values */
void print_debug_board (int **board) {
  for (int i = 0; i < SIZE; i++) {
    printf("\n");
    for (int j = 0; j < SIZE; j++) {
      printf("%d ", board[i][j]);
    }
  }
  printf("\n");
}

/* Takes event human-readable values and converts into encoded message
 * for transmission */
message_t encode_message (events_enum action, char pos_i, char pos_j) {
  message_t message;

  message.content[0] = action;
  message.content[1] = '0' + pos_i;
  message.content[2] = '0' + pos_j;
  message.content[3] = '0';
  message.content[4] = '\0';

  return message;
}

/* Takes an encoded message and returns human readable values */
move_t decode_message (event_t *event) {
  move_t move;

  move.pos_i = event->message[1] - '0';
  move.pos_j = event->message[2] - '0';
  move.value = event->message[3] == '2' ? -1 : 1;

  return move;
}

/* Takes oponent movement and updates this clients board with oponents
 * movement */
void handle_update_board (client_match_t *match, event_t *event) {
  move_t rec_move = decode_message(event);
  int error = 0;
  make_move(&error, match->board, rec_move);
}

/* Game loop and setup for dealing with user input and messages
 * comming from the server */
void exec_game (client_match_t *match, event_t *event) {
  // Enum keeps track of which turn is it
  enum {
    my_turn,
    enemy_turn,
    none,
  } game_state = none;

  // UI cursor
  int cursor[2] = {1, 1}; // { row, column }

  // Game loop.
  while (match->is_running) {

    draw_board(match->board, cursor);

    // User input. There is a timeout so that getch() returns NULL
    // if input was not given in 50 miliseconds. This timeout is
    // not for user timing, but for non-blocking processing.
    char c = getch();
    if (c != ERR) {
      switch (c) {
      case 'a': // move left
        cursor[1] = MAX(cursor[1] - 1, 0);
        break;
      case 'd': // move right
        cursor[1] = MIN(cursor[1] + 1, 2);
        break;
      case 'w': // move up
        cursor[0] = MAX(cursor[0] - 1, 0);
        break;
      case 's': // move down
        cursor[0] = MIN(cursor[0] + 1, 2);
        break;
      case ' ': // chose to play in place under cursor
        match->play_intent.valid = true;
        match->play_intent.row = cursor[0];
        match->play_intent.col = cursor[1];
        break;
      case 'q': // quit game
        // Exitting.
        match->is_running = false;
        draw_msg("Exiting...");
        // Warn other player I'm exiting
        send_message(match->connection, encode_message(quit, 0, 0).content);
      default:
        break;
      }
    }

    // If player wants to send a new move.
    if (match->play_intent.valid) {
      match->play_intent.valid = false;
      if (game_state == my_turn) {
        // Send move
        bool error = send_message(match->connection,
                                  encode_message(play, // is the event type.
                                                 match->play_intent.row,
                                                 match->play_intent.col)
                                      .content);

        // If message was not sent, server is not connected to client anymore.
        // Exit gracefully
        if (error) {
          draw_msg("Could not connect to server. Press any button to quit.");
          ui_pause(5000);
          match->is_running = false;
        }
      }
    }

    // If there is a message from the server
    if (event->is_new) {
      // Mutex so the listener thread doesn't overwrite event.
      pthread_mutex_lock(match->mutex);

      events_enum action = event->message[0];

      // Should the client update the board
      if (event->message[0] == 'U')
        handle_update_board(match, event);

      switch (action) {
      case update_board:
        break;
      case finding_match: // player entered queue
        draw_msg("Finding match...");
        break;
      case match_found: // match started, waiting for server setup
        draw_msg("Match found!");
        break;
      // this player played on invalid place and for some
      // and it was not caugh in client checking
      case invalid_move:
        draw_msg("Invalid movement, please play again");
        break;
      case your_turn: // this player can now play
        game_state = my_turn;
        draw_msg("It's your turn");
        break;
      case wait_turn: // enemy is playing, this player cannot play
        game_state = enemy_turn;
        draw_msg("Wait enemy turn");
        break;
      case win: // this player won, exit gracefully after message
        draw_msg("You won! Press any button to quit.");
        ui_pause(5000);
        match->is_running = false; // Exitting.
        break;
      case lose: // this player lost, exit gracefully after message
        draw_msg("You lose! Press any button to quit.");
        ui_pause(5000);
        match->is_running = false; // Exitting.
        break;
      case draw: // no winners, exit gracefully after message
        draw_msg("Draw! Press any button to quit.");
        ui_pause(5000);
        match->is_running = false; // Exitting.
        break;
      case quit: // oponent has left the connection, exit gracefully
        draw_msg("Opponent left. Press any button to quit ...");
        ui_pause(5000);
        match->is_running = false; // Exitting.
        break;
      default:
        break;
      }

      // Reset lockers for next event/move
      event->is_new = false;
      pthread_mutex_unlock(match->mutex);
    }
  }
}

/* Frees all heap allocated memory and closes connections
 * before game exits */
void destroy_client_match(client_match_t *match) {
  free(match->mutex);
  destroy_board(match->board);
  close(match->connection);
  free(match);
}

/* Cleans memory and closes curses interface for exiting game
 * gracefully */
void end_game(client_match_t *match, client_listener_t *client_listener){
  destroy_client_match(match);
  ui_close();
  free(client_listener);
}

/* Does the setting up, allocating all resources and firing the
 * listener thread before calling the game loop. Once the game
 * loop returns, the game has ended and cleaning is made. */
void start_game(int connection) {
  // Allocation of resources
  event_t event;
  pthread_t listener_thread;
  client_match_t *match = create_client_match(connection);
  client_listener_t *client_listener = create_client_listener(connection, match, &event);
  client_listener->event->is_new = false;

  // Firing listener thread
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&listener_thread, NULL, listener, client_listener);

  // Start game loop
  ui_init();
  exec_game(match, &event);

  // Clean up
  pthread_cancel(listener_thread);
  end_game(match, client_listener);
}
