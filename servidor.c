// UDP
#ifdef _WIN32
    #include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
    #include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
    #include <Windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <time.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGBUFSIZE 1024 // define tamanho maximo UDP

// TCP
#include <netdb.h>
#define MAX 65536  // define tamanho maximo TCP
#define PORT 8080  // definicao de porta TCP
#define SA struct sockaddr

//TCP
// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	int n, i;
	// infinite loop for chat
	//for (;;) {
  
  printf("iniciando testes de mensagens entre servidor e cliente...\n");
  
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
		printf("inserir mensagem para o cliente:");
    while ((buff[n++] = getchar()) != '\n')
			;
    */
		// and send that buffer to client		
    write(sockfd, buff, sizeof(buff));  // responde com um eco da mensagem recebida
    printf("mensagem para o cliente TCP: %s\n", buff);    
    
    // nao esta saindo por aqui... verificar...
		
    // if msg contains "Exit" then server exit and chat ended.		
    if (strncmp("exit", buff, 4) == 0) {	
			printf("servidor TCP saiu...\n");
			break;
		}    
	}
}



int main(int argc, char *argv[])
{

	
	// INICIO DO UDP	
	int i=0, j=1;
    
    char *message = "ack do servidor";
    
    if (argc != 3) {
       printf("Command line args should be multicast group and port\n");
       printf("(e.g. for SSDP, `listener 239.255.255.250 1900`)\n");
       return 1;
    }

    char* group = argv[1]; // e.g. 239.255.255.250 for SSDP
    int port = atoi(argv[2]); // 0 if error, which is an invalid port

	
#ifdef _WIN32
    //
    // Initialize Windows Socket API with given VERSION.
    //
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData)) {
        perror("WSAStartup");
        return 1;
    }
#endif

    // create what looks like an ordinary UDP socket
    //
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    // allow multiple sockets to use the same PORT number
    //
    u_int yes = 1;
    if (
        setsockopt(
            fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)
        ) < 0
    ){
       perror("Reusing ADDR failed");
       return 1;
    }

    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
    addr.sin_port = htons(port);

    // bind to receive address
    //
    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // use setsockopt() to request that the kernel join a multicast group
    //
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (
        setsockopt(
            fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)
        ) < 0
    ){
        perror("setsockopt");
        return 1;
    }

    // now just enter a read-print loop
    //
    printf("servidor aguardando mensagem multicast do orquestrador...\n");
	  while (1) {
      char msgbuf[MSGBUFSIZE];
      int addrlen = sizeof(addr);
      int nbytes = recvfrom(
        fd,
        msgbuf,
        MSGBUFSIZE,
        0,
        (struct sockaddr *) &addr,
        &addrlen
       );
      if (nbytes < 0) {
        perror("recvfrom");
        return 1;
      }
      msgbuf[nbytes] = '\0';
      puts(msgbuf);
      if (msgbuf[0] == 's') {  // testando com primeira letra de 's' de shutdown
        printf("recebendo shutdown do orquestrador e desligando o servidor...\n");
        return 0;
        break;
      }      
        
      // envia o ack do servidor para o orquestrador e cria o socket TCP
		  while (j) {  // limita o loop a executar somente uma vez
		    for (i=0;i<1;i++) { // numero de mensagens enviadas
				  char ch = 0;
				  int nbytes = sendto(
					  fd,
					  message,
					  strlen(message),
					  0,
					  (struct sockaddr*) &addr,
					  sizeof(addr)
				  );
				  if (nbytes < 0) {
					  perror("sendto");
					  return 1;
				  }
				  // TCP
				  int sockfd, connfd, len;
				  struct sockaddr_in servaddr, cli;

				  // socket create and verification
				  sockfd = socket(AF_INET, SOCK_STREAM, 0);
				  if (sockfd == -1) {
					  printf("criacao do socket falhou...\n");
					  exit(0);
                  } else {
					  printf("socket criado com sucesso..\n");
                  }
				  bzero(&servaddr, sizeof(servaddr));

				// assign IP, PORT
				servaddr.sin_family = AF_INET;
				servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
				servaddr.sin_port = htons(PORT);

				// Binding newly created socket to given IP and verification
				if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
					printf("vinculo do socket falhou...\n");
					exit(0);
                } else {
					printf("socket vinculado com sucesso..\n");
                }

				// Now server is ready to listen and verification
				if ((listen(sockfd, 5)) != 0) {
					printf("escuta do servidor TCP falhou...\n");
					exit(0);
                } else {
					printf("servidor TCP escutando...\n");
                }
				len = sizeof(cli);

				// Accept the data packet from client and verification
				connfd = accept(sockfd, (SA*)&cli, &len);
				if (connfd < 0) {
					printf("aceite do servidor TCP falhou...\n");
					exit(0);
                } else {
                    printf("o servidor TCP aceitou o cliente TCP...\n");
                }
				// FIM DO TCP     
				
        // envio de mensagem de 'terminei' multicast       
        printf("enviando mensagem de terminei do servidor...\n");
        char *message = "terminei do servidor";       
        ch = 0;
				nbytes = sendto(
					fd,
					message,
					strlen(message),
					0,
					(struct sockaddr*) &addr,
				  sizeof(addr)
				);
				if (nbytes < 0) {
				  perror("sendto");
					return 1;
				}			          
        
				// TCP 
				// Function for chatting between client and server
				func(connfd);
        

        
        
				// After chatting close the socket
				close(sockfd);        
  
			}
			j = 0;
        }       
    }
	 


#ifdef _WIN32
    //
    // Program never actually gets here due to infinite loop that has to be
    // canceled, but since people on the internet wind up using examples
    // they find at random in their own code it's good to show what shutting
    // down cleanly would look like.
    //
    WSACleanup();
#endif
    return 0;
}
