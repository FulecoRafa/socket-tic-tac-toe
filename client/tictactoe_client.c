#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../helpers/macros.h"

pthread_mutex_t mutexsum = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[2];

void *listener(void * args) {
  int *client_socket = (int*) args;
  int recebidos;
  char resposta[256];
  do {
    recebidos = recv(*client_socket, resposta, 256, 0);
    resposta[recebidos] = '\0';
    printf("\n Servidor: %s\n", resposta);
  } while (recebidos != -1);
}

int main() {

  int client_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (client_socket == -1) {
    eprintf("Could not create a connection :(\n");
    return 1;
  }

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

  printf("Looking for a match...\n");

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // pthread_create(&threads[0], &attr, sendmessage, NULL);
  pthread_create(&threads[1], &attr, listener, &client_socket);

  int i;
  scanf("%d", &i);
  printf("%ld\n", send(client_socket, "Hello", 6, 0));

  while (true) {}

  return 0;
}

// void *sendmessage(){
// int  enviados;
// char mensagem[256];

// do{
//   printf("Cliente: ");
//   fgets(mensagem,256,stdin);
//   mensagem[strlen(mensagem)-1] = '\0';
//   enviados = send(client_socket,mensagem,strlen(mensagem),0);

// }while(strcmp(mensagem,"exit")!=0);

// 	pthread_mutex_destroy(&mutexsum);
// 	pthread_exit(NULL);
// 	close(client_socket);
// 	done=0;
// }
