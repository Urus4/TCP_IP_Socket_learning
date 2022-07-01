#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	int str_len;
	char message[100];
	int fd ;

	if(argc!=3)
	{
		printf("Usage ; %s <port> <rcv file>\n", argv[0]);
		exit(1);
	}

	fd = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC);
	if(fd<0)// 리눅스에서는 0보다 작으면 오류
		error_handling("open() error!");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	int optvalue=1;
	setsockopt(serv_sock,SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	clnt_addr_size=sizeof(clnt_addr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if(clnt_sock==-1)
		error_handling("accept() error");
	//else printf("complete connect!");
	
	while(1)
	{
		if((str_len=read(clnt_sock,message,sizeof(message)))!=0)
		{
			write(fd, message,str_len);
			printf("read : %d\n",str_len);
		}
		else break;
	}
	printf("Done..");
	
	close(fd);
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
