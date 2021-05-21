//
// Created by Gabriel Souza on 18/04/21.
//

#include "ui.h"
#include <ncurses.h>
#include "match.h"
#include "../helpers/message.h"

//int has_message = 0;

void delay(int time_in_millis) {
  clock_t start_time = clock();
  while (clock() < start_time + time_in_millis);
}

void *listener(void *args) { // Listener thread function for client.
  client_listener_t *client_listener = (client_listener_t *) args;
  int size;
  char message[5];
  
  //char msg_buf[64];
  do {
    pthread_mutex_lock(client_listener->mutex);
    
    if(client_listener->event->is_new == false) {
    size = recv((int) client_listener->connection, message, 5, 0);
    message[size] = '\0';
      if (size > 0) {
        //printf("Received: %d %s\n", size, message);
        //fprintf(stderr, "Received: [%d][%s]\n", size, message);
        //while (has_message);
        strcpy(client_listener->event->message, message);
        //has_message = 1;
        client_listener->event->is_new = true;
        // if (client_listener->event->message[0] == 'U') fprintf(stdout, "Listener: update_board >%s<[%d]\n", client_listener->event->message, client_listener->event->is_new);
      }
    }
    
    pthread_mutex_unlock(client_listener->mutex);
  } while (size != -1);
  return NULL;
}

void *send_message(int client_connection, char *message) {
  char msg_buf[64];
  int bytes_sent = send(client_connection, message, strlen(message), 0);
  sprintf(msg_buf, "Sent %d %s\n", bytes_sent, message);
  draw_msg(msg_buf);
  return NULL;
}

client_match_t *create_client_match(int connection) {
  client_match_t *match = (client_match_t *) calloc(1, sizeof(client_match_t));
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

message_t encode_message(events_enum action, char pos_i, char pos_j) {
  message_t message;
  message.content[0] = action;
  message.content[1] = '0' + pos_i;
  message.content[2] = '0' + pos_j;
  message.content[3] = '0';
  message.content[4] = '\0';
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
  /*
  char pos_i, pos_j;
  
  char msg_buf[64];
  sprintf(msg_buf, "It's your turn!");
  draw_msg(msg_buf);*/

  //print_debug_board(match->board);
  //scanf("%c %c", &pos_i, &pos_j);
  //fflush(stdin);
  /*
  char pos_i = match->play_intent.row;
  char pos_j = match->play_intent.col;
                                                //Play is the event.
  send_message(match->connection, encode_message(play, pos_i, pos_j));*/
}

void handle_update_board(client_match_t *match, event_t *event) {
  move_t rec_move = decode_message(event);
  //printf("\n\nMoving %d on %d %d\n", rec_move.value, rec_move.pos_i, rec_move.pos_j);
  int error = 0;
  make_move(&error, match->board, rec_move);
}

void exec_game(client_match_t *match, event_t *event) {

  enum {
    my_turn,
    enemy_turn,
    none,
  } game_state = none;

  // Cursor and UI stuff.
  int cursor[2] = { 1, 1 }; //Position 0 is for row, 1 is for column.
      
  while (match->is_running) { //Game loop.
  /*  if (event.is_new) {
      pthread_mutex_lock(match->mutex);
      exec_game(match, &event);
      pthread_mutex_unlock(match->mutex);
    }*/

    draw_board(match->board, cursor);

    char c = getch();
    if (c != ERR) {
      switch (c) {
      case 'a':
        cursor[1] = MAX(cursor[1] - 1, 0);
        break;
      case 'd':
        cursor[1] = MIN(cursor[1] + 1, 2);
        break;
      case 'w':
        cursor[0] = MAX(cursor[0] - 1, 0);
        break;
      case 's':
        cursor[0] = MIN(cursor[0] + 1, 2);
        break;
      case ' ':
        draw_msg("Tried to play");
        match->play_intent.valid = true;
        match->play_intent.row = cursor[0];
        match->play_intent.col = cursor[1];
        break;
      case 'm':
        draw_msg("This is a message");
        break;
      case 'e':
        draw_msg("This is an error");
        break;
      case 'q':
        ui_close();
        //#dealloc
        match->is_running = false; //Exitting.
      default:
        break;
      }
    } /*else {
      fprintf(stderr, "Curses: No input\n");
    }*/

    if (match->play_intent.valid) { //If player sent a new move.
      match->play_intent.valid = false;
      if (game_state == my_turn)
      {
        draw_msg("You played");
        
        // Handle play.
        send_message(
          match->connection,
          encode_message(
            play, //is the event type.
            match->play_intent.row,
            match->play_intent.col).content
          );
      }
    }
    
    if (event->is_new) {
      pthread_mutex_lock(match->mutex); //Mutex so the listener thread doesn't overwrite event.
  
      events_enum action = event->message[0];
      //fprintf(stderr, ">>>Message is %s >>%d<<\n", event->message, action);
      if (event->message[0] == 'U')
      {
          //printf("UPDATING BOARD");
          handle_update_board(match, event);
      }

      switch (action) {
        case update_board:
          break;
        case finding_match:
          draw_msg("Finding match...");
          break;
        case match_found:
          draw_msg("Match found!");
          break;
        case invalid_move: //TODO fix.
          draw_msg("Invalid movement, please play again");
          break;
        case your_turn:
          game_state = my_turn;
          draw_msg("It's your turn");
          break;
        case wait_turn:
          game_state = enemy_turn;
          draw_msg("Wait enemy turn");
          break;
        case win:
          draw_msg("You won!");
          break;
        case lose:
          draw_msg("You lose");
          break;
        case draw:
          draw_msg("Draw");
          break;

        default:
          break;
      }
      
      event->is_new = false;
      //has_message = 0;
      pthread_mutex_unlock(match->mutex);
    }
  }
}

void destroy_client_match(client_match_t *match){
    free(match->mutex);
    destroy_board(match->board);
    free(match);
}

void end_game(client_match_t *match, client_listener_t *client_listener){
    destroy_client_match(match);
    free(client_listener);
}

void start_game(int connection) {
  event_t event;
  pthread_t listener_thread;
  client_match_t *match = create_client_match(connection);
  client_listener_t *client_listener = create_client_listener(connection, match, &event);
  client_listener->event->is_new = false;
  
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&listener_thread, NULL, listener, client_listener);

  ui_init();
  exec_game(match, &event);

  //TODO join threads
  end_game(match, client_listener);
}
