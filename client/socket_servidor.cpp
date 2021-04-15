#include <stdio.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>


int    sock_cliente;
struct sockaddr_in addr;
pthread_mutex_t mutexsum = PTHREAD_MUTEX_INITIALIZER;  
void *sendmessage();
void *listener();
int done=1; 

int main()
{ 
int    meu_socket;
meu_socket = socket(AF_INET,SOCK_STREAM,0);

if(meu_socket == -1)
  {
  printf("\nErro ao criar o socket!\n");
  return 1;
  }

addr.sin_family      = AF_INET;
addr.sin_port        = htons(1234);
addr.sin_addr.s_addr = INADDR_ANY;
memset(&addr.sin_zero,0,sizeof(addr.sin_zero));

if(bind(meu_socket,(struct sockaddr*)&addr,sizeof(addr)) == -1)
  {
  printf("\nErro na funcao bind()\n");
  return 1;
  }

if(listen(meu_socket,1) == -1)
  {
  printf("\nErro na funcao listen()\n");
  return 1;
  }

printf("\nAguardando cliente...\n");

sock_cliente = accept(meu_socket,0,0);

if(sock_cliente == -1)
  {
  printf("\nErro na funcao accept()\n");
  return 1;
  }

printf("\nCliente conectado!\n");
pthread_t threads[2];
    void *status;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &attr, sendmessage, NULL);
    pthread_create(&threads[1], &attr, listener, NULL);



while(done){}
        

return 0;

}

void *sendmessage(){
int  enviados;
char mensagem[256];

do{  
  printf("Server: ");
  fgets(mensagem,256,stdin);
  mensagem[strlen(mensagem)-1] = '\0';
  enviados = send(sock_cliente,mensagem,strlen(mensagem),0);
}while(strcmp(mensagem,"exit")!=0);
}

void *listener(){
int recebidos;
char resposta[256];
do{
  recebidos = recv(sock_cliente,resposta,256,0);              /* Recebe mensagem do cliente */
  resposta[recebidos] = '\0';
  printf("\n Cliente: %s\n",resposta);
  }while(strcmp(resposta,"exit")!=0); 
	pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
	done=0;
}
