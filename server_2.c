#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv){

    if(argc!=4){
        printf("Usage: %s <ReceiverPort> <SenderPort> <PacketDropProbability>", argv[0]);
        exit(0);
    }
    // check number of arguments

    char *ip = "127.0.0.1"; // for local machine, ip hardcoded
    int rport = atoi(argv[1]);
    int sport = atoi(argv[2]);
    double pdprob = atof(argv[3]);

    int sockfdsend, sockfdrecv; // 2 socket descriptors
    struct sockaddr_in server_addr_send, client_addr_send;
    struct sockaddr_in server_addr_recv, client_addr_recv;
    char buffer[1024];
    socklen_t addr_size;
    int n, m;

    // printf("%d %d %f", rport, sport, pdprob);

    sockfdsend = socket(AF_INET, SOCK_DGRAM, 0);
    sockfdrecv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfdsend < 0 || sockfdrecv < 0){
        perror("[-]socket error");
        exit(1);
    }

    memset(&server_addr_send, '\0', sizeof(server_addr_send));
    server_addr_send.sin_family = AF_INET;
    server_addr_send.sin_port = htons(sport);
    server_addr_send.sin_addr.s_addr = inet_addr(ip);

    memset(&server_addr_recv, '\0', sizeof(server_addr_recv));
    server_addr_recv.sin_family = AF_INET;
    server_addr_recv.sin_port = htons(rport);
    server_addr_recv.sin_addr.s_addr = inet_addr(ip);

    m = bind(sockfdrecv, (struct sockaddr*)&server_addr_recv, sizeof(server_addr_recv));
    n = bind(sockfdsend, (struct sockaddr*)&server_addr_send, sizeof(server_addr_send));
    
    if (n < 0 || m < 0) {
        perror("[-]bind error");
        exit(1);
    }
    
    int c = 1;
    while(1){
        bzero(buffer, 1024);
        addr_size = sizeof(client_addr_recv);
        int i = recvfrom(sockfdrecv, buffer, 1024, 0, (struct sockaddr*)&client_addr_recv, &addr_size);
        printf(buffer);
        printf("\n");

        char out[i-6];
        strncpy(out,&buffer[7],i-7);
        out[i-7] = '\0';
        int outp = atoi(out);
        // printf("%d %d\n", outp, c);

        if(outp == c){
            double randomno = (double)rand()/ (double)RAND_MAX;
            if(randomno < pdprob) {
                printf("Packet Dropped\n");
                continue;
            }
            c++;
        }

        bzero(buffer, 1024);
        sprintf(buffer, "Acknowledgment:%d", c);
        sendto(sockfdsend, buffer, 1024, 0, (struct sockaddr*)&client_addr_send, sizeof(client_addr_send));
        printf(buffer);
        printf("\n");
    }
    return 0;
}