#include "unp.h"

int main(int argc, char** argv)
{
    int sockfd, n;
    char recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr_storage ss;

    if(argc != 3)
        err_quit(
            "usage: %s <hostname/IPaddress> <service/port#>", argv[0]);

    sockfd = tcp_connect(argv[1], argv[2]);

    len = sizeof(ss);
    Getpeername(sockfd, (SA*)&ss, &len);
    printf("connected to %s\n", Sock_ntop_host((SA*)&ss, len));

    while((n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
    exit(0);
}

