#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../helpers/macros.h"
#include "match.h"

/* Function runs every time program exits with SIGTERM */
void exiting () {
  printf("Exiting...\n");
}

/* Instantiates connection and passes details to match */
int main() {

  // Start socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    eprintf("Could not create a connection :(\n");
    return 1;
  }

  // Connect socket to server
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6060);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

  printf("Connecting...\n");
  if (connect(client_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    eprintf("Could not connect to server, try again later :(\n");
    return 1;
  }

  printf("Conected!\n\n");

  // Handles game logic
  start_game(client_socket);

  return 0;
}
