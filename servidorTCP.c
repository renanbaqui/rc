#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 512  // define tamanho maximo do char
#define PORT 8080  // definicao de porta
#define SA struct sockaddr

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	int i;
	// infinite loop for chat
	//for (;;) {
 
  for (;;) {
		
    bzero(buff, MAX); // zera o buffer

		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));	
		// print buffer which contains the client contents
		printf("mensagem do cliente TCP: %s\t", buff);
		
    //bzero(buff, MAX);
		/*
    n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;
    */
		// and send that buffer to client
		
    write(sockfd, buff, sizeof(buff));  // responde com um eco da mensagem recebida
    printf("mensagem para o cliente TCP: %s\n", buff);

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {	// substituir por shutdown'?
			printf("servidor TCP saiu...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("criacao do socket falhou...\n");
		exit(0);
	}
	else
		printf("socket criado com sucesso..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("vinculo do socket falhou...\n");
		exit(0);
	}
	else
		printf("socket vinculado com sucesso..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("escuta do servidor TCP falhou...\n");
		exit(0);
	}
	else
		printf("servidor TCP escutando...\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("aceite do servidor TCP falhou...\n");
		exit(0);
	}
	else
		printf("o servidor TCP aceitou o cliente TCP...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}