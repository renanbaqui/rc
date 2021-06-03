#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 256  // define tamanho maximo
#define PORT 8080 // definicao de porta
#define SA struct sockaddr

#include <sys/time.h>
#include <unistd.h>

char mensagem[MAX];


void func(int sockfd, int tamanho, int repeticoes)
{
	int i, n;
  
  // char mensagem[] = "03 01 07 04"; // 
    
  // loop de criacao da mensagem no tamanho desejado
  for(i=0; i<tamanho; i++) {
    mensagem[i] = '1';
    //if (i==tamanho-1) {
    //  mensagem[i] = '\0';
    //}  
  }
  
  char buff[MAX]; // MAX tamanho em bytes da mensagem? // subtituir por array de bytes? // definir como 'ack'?
	 
  //bzero(buff, sizeof(buff)); // zera o buffer
	
  // loop de envio e recebimento de mensagens
  for(i=0; i<repeticoes; i++)  // repete o envio e recebimento de string 'repeticoes' vezes 
  {
    printf("Mensagem enviada : %s numero %d\n", mensagem, i);
		

		// envia a mensagem
    write(sockfd, mensagem, sizeof(mensagem));
		
    /*
    n = 0; 
		while ((buff[n++] = getchar()) != '\n');
    bzero(buff, sizeof(buff));		
    */
    
    // le a mensagem
    read(sockfd, buff, sizeof(buff));
		printf("Mensagem recebida : %s\n", buff);
	  
     
    // if ((strncmp(buff, "exit", 4)) == 0) { // substituir 'exit' por 'shutdown'?
	  // printf("Client Exit...\n");
	} 
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
  
  struct timeval start, end;
  long mtime, secs, usecs;  
	
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
  
  gettimeofday(&start, NULL); // inicio da contagem do tempo
  
	// function for chat
	func(sockfd, 16, 16);				// loop enviando uma mensagem com um tamanho configurado, aguardando a resposta, calculando o tempo de RTT?
						                  // criar outro loop? - ao iniciar, o cliente fica aguardando uma mensagem de multicast do orquestrador.

  gettimeofday(&end, NULL);  // fim da contagem do tempo  
  
  secs  = end.tv_sec  - start.tv_sec;
  usecs = end.tv_usec - start.tv_usec;
  mtime = ((secs) * 1000 + usecs/1000.0) + 0.5;
  printf("Tempo decorrido: %ld milissegundos\n", mtime);
  
  // close the socket
	close(sockfd);
 

}