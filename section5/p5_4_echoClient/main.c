#include "unp.h"

void str_cli(FILE* fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while(Fgets(sendline, MAXLINE, fp) != NULL){
		Writen(sockfd, sendline, strlen(sendline));

		if(Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");
		
		Fputs(recvline, stdout);
	}
}

int main(int argc, char** argv)
{
	int i, sockfd[5];
	struct sockaddr_in servaddr;

	if(argc != 2)
		err_quit("usage: main <IPaddress>");

	for(i = 0; i < 5; i++){
		sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		Connect(sockfd[i], (SA*)&servaddr, sizeof(servaddr));
	}

	str_cli(stdin, sockfd[0]);

    exit(0);
}
