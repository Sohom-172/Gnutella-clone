#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<time.h>


int main(int argc, char **argv){
    
    if(argc!=5){
        printf("Usage: %s <SenderPort> <ReceiverPort> <RetransmissionTimer> <NoOfPacketsToBeSent>", argv[0]);
        exit(0);
    }
    // check number of arguments

    char *ip = "127.0.0.1"; // for local machine, ip hardcoded
    
    int sport = atoi(argv[1]);
    int rport = atoi(argv[2]);
    int retimer = atof(argv[3]);
    int numpkts = atof(argv[4]);
    
    int sockfdsend, sockfdrecv; // 2 socket descriptors
    struct sockaddr_in addr_send;
    struct sockaddr_in addr_recv;
    char buffer[1024]; // buffer of 1024
    socklen_t addr_size;
    int n, m;

    // printf("%d %d\n", rport, sport);

    sockfdsend = socket(AF_INET, SOCK_DGRAM, 0);
    sockfdrecv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfdsend < 0 || sockfdrecv < 0){
        perror("[-]socket error");
        exit(1);
    }


    memset(&addr_send, '\0', sizeof(addr_send));
    addr_send.sin_family = AF_INET;
    addr_send.sin_port = htons(sport);
    addr_send.sin_addr.s_addr = inet_addr(ip);

    memset(&addr_recv, '\0', sizeof(addr_recv));
    addr_recv.sin_family = AF_INET;
    addr_recv.sin_port = htons(rport);
    addr_recv.sin_addr.s_addr = inet_addr(ip);

    for(int i = 1; i<=numpkts; i++){
        bzero(buffer, 1024);
        sprintf(buffer, "Packet:%d", i);
        sendto(sockfdsend, buffer, 1024, 0, (struct sockaddr*)&addr_send, sizeof(addr_send));
        printf(buffer);
        printf("\n");

        struct timeval timeout;      
        timeout.tv_sec = retimer;
        timeout.tv_usec = 0;
        bzero(buffer, 1024);
        addr_size = sizeof(addr_recv);
        setsockopt(sockfdrecv, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof timeout);
        int A = recvfrom(sockfdrecv, buffer, 1024, 0, (struct sockaddr*)&addr_recv, &addr_size);
        printf(buffer);
        printf("\n");

        if(A <= 0){
            i--;
            continue;
        }

        char out[A-14];
        strncpy(out,&buffer[15],A-15);
        out[A-15] = '\0';
        int outp = atoi(out);

        if(outp != i+1){
            i--;
            continue;
        }        
    }
    return 0;
}