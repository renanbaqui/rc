#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080 // definicao de porta
#define SA struct sockaddr

#include <sys/time.h>
#include <unistd.h>

void func(int sockfd)
{
	const char mensagem[MAX] = "teste";
  char buff[MAX]; // MAX tamanho em bytes da mensagem? // subtituir por array de bytes? // definir como 'ack'?
	int n, m = 4;
  int i;
	for (;;) {
		//bzero(buff, sizeof(buff)); // zera o buffer
		for(i=0; i<30; i++)  // repete o envio de string 30 vezes 
    {
      printf("String enviado : %s %d\n", mensagem, i);
  		/*n = 0;
  		while ((buff[n++] = getchar()) != '\n')
  			;*/
  		write(sockfd, mensagem, sizeof(mensagem));
  		//bzero(buff, sizeof(buff));
  		read(sockfd, buff, sizeof(buff));
  		printf("String recebido : %s\n", buff);
		//if ((strncmp(buff, "exit", 4)) == 0) { // substituir 'exit' por 'shutdown'?
		//	printf("Client Exit...\n");
		}
    break;		
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
  
  struct timeval start, end;
  long mtime, secs, usecs;
  
  gettimeofday(&start, NULL); // inicio tempo
	
  // criacao do socket e verificacao
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("criacao do socket falhou...\n");
		exit(0);
	}
	else
		printf("socket criado com sucesso...\n");
	bzero(&servaddr, sizeof(servaddr));

	// atribuicao de IP e PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("152.92.236.17"); // original 127.0.0.1
	servaddr.sin_port = htons(PORT);

	// conecta o socket do client ao socket do servidor
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("conexao com o servidor falhou...\n");
		exit(0);
	}
	else
		printf("conectado ao servidor..\n");
  
  
	// function for chat
	func(sockfd);				// loop enviando uma mensagem com um tamanho configurado, aguardando a resposta, calculando o tempo de RTT?
						          // criar outro loop? - ao iniciar, o cliente fica aguardando uma mensagem de multicast do orquestrador.

  gettimeofday(&end, NULL);  // fim do tempo
  secs  = end.tv_sec  - start.tv_sec;
  usecs = end.tv_usec - start.tv_usec;
  mtime = ((secs) * 1000 + usecs/1000.0) + 0.5;
  printf("Tempo decorrido: %ld milissegundos\n", mtime);
  
  // close the socket
	close(sockfd);
 

}