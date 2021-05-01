#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Thread
#include <pthread.h>

#include "../helpers/macros.h"
#include "match.h"

#define MAX_PLAYERS 10

struct sockaddr_in addr;
pthread_t game_threads[100];
int thread_count = 0;

// This function executes before app exits and does some cleanup
// This force quits all threads to garantee memory optimization
// on out of order exits
void exiting() {
  forEachInArray(pthread_t, game_threads,
                 lambda(pthread_t, (pthread_t * game_thread), {
                   pthread_cancel(*game_thread);
                   return *game_thread;
                 }));
}

int main(int argc, char *argv[]) {

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket == -1) {
    eprintf("Could not create server socket :(\n");
    return 1;
  }

  addr.sin_family = AF_INET;

  if (argc < 2)
  {
    addr.sin_port = htons(6060);
  }
  else
  {
    addr.sin_port = htons(atoi(argv[1]));
  }
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

  if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    eprintf("Could not bind server socket :(\n");
    return 1;
  }

  if (listen(server_socket, 1) == -1) {
    eprintf("Server could not listen :(\n");
    return 1;
  }

  printf("🚀 Tic tac toe server up and running @ localhost: %d\n", addr.sin_port);

  match_making(server_socket, MAX_PLAYERS, game_threads, &thread_count);

  return 0;
}