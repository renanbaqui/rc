#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 256  // define tamanho maximo do char
#define PORT 8080 // definicao de porta
#define SA struct sockaddr

#include <sys/time.h>  // contagem do tempo
#include <unistd.h>    // contagem do tempo

#include <sys/types.h> // recv()

char mensagem[MAX];

int byte_count;

void teste(int sockfd, int tamanho, int repeticoes)
{	
  
  int i, n;
  
  const char mensagem[] = "03 01 07 04"; // 
  /*  
  // loop de criacao da mensagem no tamanho desejado  
  for(i=0; i<tamanho; i++) {
    mensagem[i] = '1';
    if (i==tamanho-1) {
      mensagem[i] = '\0';
    }  
  }
  */
  char buff[MAX]; // MAX tamanho em bytes da mensagem
	 
  bzero(buff, sizeof(buff)); // zera o buffer
	
  // loop de envio e recebimento de mensagens
  for(i=0; i<repeticoes; i++)  // repete o envio e recebimento de string 'repeticoes' vezes 
  {
    
		// envia a mensagem e imprime
    write(sockfd, mensagem, sizeof(mensagem));
		printf("mensagem enviada para o servidor: %s numero %d\n", mensagem, i);    
    
    //n = 0; 
		//while ((buff[n++] = getchar()) != '\n');
    bzero(buff, sizeof(buff));  // zera o buffer    
    
    // le a mensagem e imprime
    read(sockfd, buff, sizeof(buff));
		printf("mensagem recebida do servidor: %s\n", buff);
	  
        
    // all right! now that we're connected, we can receive some data!
    byte_count = recv(sockfd, buff, sizeof(buff), 0);
    printf("recv()'d %d bytes de dados no buff\n", byte_count);
    
    
    // termina a conexao no final do loop?
    if (i == repeticoes-1) {      
      char mensagem[] = "exit";
      write(sockfd, mensagem, sizeof(mensagem));
      char buff[] = "exit";
      puts(buff);
    }     
    
    if ((strncmp(buff, "exit", 4)) == 0) { // substituir 'exit' por 'shutdown'?
	    printf("Client Exit...\n");
    }
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

	// conecta o socket do cliente ao socket do servidor
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("conexao com o servidor falhou...\n");
		exit(0);
	}
	else
		printf("conectado ao servidor...\n");
  
  gettimeofday(&start, NULL); // inicio da contagem do tempo
  
	// function for chat
	teste(sockfd, 16, 16);				// loop enviando uma mensagem com um tamanho configurado, aguardando a resposta, calculando o tempo de RTT?
						                  // criar outro loop? - ao iniciar, o cliente fica aguardando uma mensagem de multicast do orquestrador.

  gettimeofday(&end, NULL);  // fim da contagem do tempo  
  
  secs  = end.tv_sec  - start.tv_sec;
  usecs = end.tv_usec - start.tv_usec;
  mtime = ((secs) * 1000 + usecs/1000.0) + 0.5;
  printf("tempo decorrido: %ld milissegundos\n", mtime);
  
  // close the socket
	close(sockfd);
 

}