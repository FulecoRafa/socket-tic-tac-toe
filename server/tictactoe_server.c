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

struct sockaddr_in addr;
pthread_t game_threads[100];

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

// Game creation
void start_game(int conns[2]) {
  printf("Starting game with %d and %d\n", conns[0], conns[1]);
}

int main() {

  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket == -1) {
    eprintf("Could not create server socket :(\n");
    return 1;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(6060);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

  if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    eprintf("Could not bind server socket :(\n");
    return 1;
  }

  if (listen(server_socket, 1) == -1) {
    eprintf("Server could not listen :(\n");
    return 1;
  }

  printf("🚀 Tic tac toe server up and running @ localhost:9000\n");

  while (true) {
    int conns[2];
    // Player connection
    conns[0] = accept(server_socket,0,0);

    if (conns[0] == -1) {
      eprintf("Someone tried to connect, but it went wrong...\n");
      return 1;
    }

    printf("Client %d has just entered queue\n", conns[0]);

    // Oponent connection
    conns[1] = accept(server_socket,0,0);

    if (conns[1] == -1) {
      printf("Someone tried to connect, but it went wrong...\n");
      return 1;
    }

    printf("Client %d has just entered queue\n", conns[1]);

    // Start game with last two connections
    start_game(conns);
  }
  return 0;
}

// void *sendmessage()
// {
//   int enviados;
//   char mensagem[256];

//   do
//   {
//     printf("Server: ");
//     fgets(mensagem, 256, stdin);
//     mensagem[strlen(mensagem) - 1] = '\0';
//     enviados = send(connected_socket, mensagem, strlen(mensagem), 0);
//   } while (strcmp(mensagem, "exit") != 0);
// }

// void *listener()
// {
//   int recebidos;
//   char resposta[256];
//   do
//   {
//     recebidos = recv(connected_socket, resposta, 256, 0); /* Recebe mensagem do cliente */
//     resposta[recebidos] = '\0';
//     printf("\n Cliente: %s\n", resposta);
//   } while (strcmp(resposta, "exit") != 0);
//   pthread_mutex_destroy(&mutexsum);
//   pthread_exit(NULL);
//   done = 0;
// }
