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
// #include <pthread.h>

// CPP libs
#include <queue>
#include <vector>

struct sockaddr_in addr;
bool should_threads_terminate = false;
std::vector<pthread_t> game_threads;

// This function executes before app exits and does some cleanup
void exiting () {
  should_threads_terminate = true;
  for (pthread_t thread : game_threads) {
    // pthread_join(thread, NULL);
  }
}

// Game creation
void start_game(struct sockaddr *conn1, struct sockaddr *conn2) {
  printf("Starting game with %s and %s\n", conn1->sa_data, conn2->sa_data);
}

int main () {
  std::atexit(exiting);

  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket == -1) {
    printf("\n[❌] Could not create server socket :(\n");
    return 1;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(6060);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

  if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    printf("\n[❌] Could not bind server socket :(\n");
    return 1;
  }

  if (listen(server_socket, 1) == -1) {
    printf("\n[❌] Server could not listen :(\n");
    return 1;
  }

  printf("🚀 Tic tac toe server up and running @ localhost:9000\n");

  while (true) {
    struct sockaddr *new_conn1, *new_conn2;
    socklen_t *new_conn_len1, *new_conn_len2;

    // Player connection
    int conn_status = accept(server_socket, new_conn1, new_conn_len1);

    if (conn_status == -1) {
      printf("\nSomeone tried to connect, but it went wrong...\n");
      return 1;
    }

    printf("Client %s has just entered queue\n", new_conn1->sa_data);

    // Oponent connection
    conn_status = accept(server_socket, new_conn2, new_conn_len2);

    if (conn_status == -1) {
      printf("Someone tried to connect, but it went wrong...\n");
      return 1;
    }

    printf("Client %s has just entered queue\n", new_conn2->sa_data);

    // Start game with last two connections
    start_game(new_conn1, new_conn2);
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
