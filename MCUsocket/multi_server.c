#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

static char * host = "localhost";
static char * user = "root";
static char * pass = "kcci";
static char * dbname = "khm";


int main(int argc, char *argv[])
{
     // < DB 연결 > ==========================
     MYSQL * conn;
     conn = mysql_init(NULL);
     int sql_index, flag=0;
     char in_sql[200] = {0};
     int res = 0;
 
     if(!(mysql_real_connect(conn, host, user, pass, dbname, 0,NULL,0)))
     {
         fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
         exit(1);
     }else
         printf("Connection Successful! \n\n");
     //========================================
	




	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc !=2) {
		printf("Usage : %s <port \n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM,0);
	if(serv_sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	int opt=1;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSERADDR, (void*)&opt, sizeof(opt));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n",clnt_cnt+1);
		
		if(clnt_cnt >= MAX_CLNT)
		{
			printf("socket full\n");
			shutdown(clnt_sock,SHUT_WR);
			continue;
		}
		else if(clnt_sock < 0)
		{
			perror("accept()");
			continue;
		}

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0,i;
	char msg[BUF_SIZE];

	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
		send_msg(msg, str_len);

	pthread_mutex_lock(&mutx);
	for(i=0 ; i<clnt_cnt; i++)
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

void send_msg(char * msg, int len)
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
 	exit(1);
}

