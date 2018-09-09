#include "unp.h"

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0)
        Writen(sockfd, buf, n);

    if(n < 0 && errno == EINTR)
        goto again;
    else if(n < 0)
        err_sys("str_echo: read error");
}

int main(int argc, char** argv)
{
    int listenfd, connfd, udpfd, nready, maxfdpl;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);

    /* Create listening TCP socket */
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    /* Create UDP socket */
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(udpfd, (SA*)&servaddr, sizeof(servaddr));

    Signal(SIGCHLD, sig_chld);

    FD_ZERO(&rset);
    maxfdpl = max(listenfd, udpfd) + 1;
    for(;;) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if((nready = select(maxfdpl, &rset, NULL, NULL, NULL)) < 0) {
            if(errno == EINTR)
                continue;
            else
                err_sys("select error");
        }

        if(FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA*)&cliaddr, &len);

            if((childpid = Fork()) == 0) {
                Close(listenfd);
                str_echo(connfd);
                exit(0);
            }
            Close(connfd);
        }

        if(FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA*)&cliaddr, &len);

            Sendto(udpfd, mesg, n, 0, (SA*)&cliaddr, len);
        }
    }
}

