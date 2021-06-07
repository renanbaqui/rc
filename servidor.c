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
#include "mensagem.h"

#define MSGBUFSIZE 1024 // define tamanho maximo UDP

// TCP
#include <netdb.h>
#define MAX 65536  // define tamanho maximo TCP
#define PORT 8822  // definicao de porta TCP
#define SA struct sockaddr

//TCP
void teste(int sockfd, mensagem_t* m_dispara)
{
    char msgbuff[MAX];
    int n, i;
    int r = repeticoes(m_dispara);
    int t = tamanho(m_dispara);

    printf("iniciando testes de mensagens entre servidor e cliente...\n");

    for (i=0; i < r; i++) {

        bzero(msgbuff, MAX); // zera o buffer

        read(sockfd, msgbuff, t);

        mensagem_t* m = (mensagem_t*) msgbuff;

        printf("mensagem %d/%d do cliente TCP: "
               "tipo %x origem %x tamanho %x repetições %x\n",
               i+1,
               r,
               m->tipo,
               m->origem,
               m->tamanho,
               m->repeticoes);

        m->origem = SERVIDOR;
        write(sockfd, msgbuff, t);  // responde com um eco da mensagem recebida
    }
}




int main(int argc, char *argv[])
{
    // INICIO DO UDP
    
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
    addr.sin_addr.s_addr = inet_addr(group); // differs from sender
    addr.sin_port = htons(port);

    // addr do servidor TCP
    //
    struct sockaddr_in servaddr, cli;
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);


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

    printf("servidor aguardando mensagem multicast do orquestrador...\n");
    while (1) {
        char msgbuf[MSGBUFSIZE];
        int addrlen = sizeof(addr);
        int nbytes = recvfrom(fd,
                              msgbuf,
                              TAMANHO_DA_MENSAGEM,
                              0,
                              (struct sockaddr *) &addr,
                              &addrlen
                              );
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        } else if (nbytes < 4){
            puts("Não recebi uma mensagem inteira\n");
            return 1;
        }

        mensagem_t* m = (mensagem_t*) msgbuf;

        if (m->origem != ORQUESTRADOR) {
            // Ignora e lê próxima mensagem
            continue;
        }

        switch (m->tipo){
        case (DISPARA):
            printf("recebendo dispara do orquestrador e preparando pra teste...\n");
            break;
        case (DESLIGA):
            printf("recebendo shutdown do orquestrador e desligando o servidor...\n");
            return 0;
        default:
            printf("recebei mensagem inválida de tipo %d, desligando o servidor...\n",
                   m->tipo);
            return -1;
        }

        ///////////////////
        // TCP (teste)

        int sockfd, connfd, len;

        // Cria o socket TCP
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("criacao do socket falhou...");
            exit(0);
        } else {
            printf("socket criado com sucesso..\n");
        }

        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes));

        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            perror("vinculo do socket falhou...\n");
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

        m->origem = SERVIDOR;
        // envia o ack do servidor para o orquestrador
        nbytes = sendto(fd,
                        msgbuf,
                        TAMANHO_DA_MENSAGEM,
                        0,
                        (struct sockaddr*) &addr,
                        sizeof(addr)
                        );
        if (nbytes < 0) {
            perror("sendto ack dispara");
            return 1;
        }

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("aceite do servidor TCP falhou...\n");
            exit(0);
        } else {
            printf("o servidor TCP aceitou o cliente TCP...\n");
        }
        // TCP
        // Function for chatting between client and server
        teste(connfd, m);


        close(connfd);
        // After chatting close the socket
        close(sockfd);


        // FIM DO TCP

        // envio de mensagem de 'terminei' multicast
        printf("servidor enviando mensagem de terminei...\n");

        m->tipo = EXITO;

        // O orquestrador interpreta repetições aqui como status de retorno
        // retorna "OK"
        m->repeticoes = 0;

        nbytes = sendto(fd,
                        msgbuf,
                        TAMANHO_DA_MENSAGEM,
                        0,
                        (struct sockaddr*) &addr,
                        sizeof(addr)
                        );
        if (nbytes < 0) {
            perror("sendto");
            return 1;
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
