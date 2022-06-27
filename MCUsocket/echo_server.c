#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mysql/mysql.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 50 
void error_handling(char *message);

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

    int temp=0,level=0;
    char state[10];
	char msg_cur[10];
	char msg_pre[10];
    char *pArray[3] = {0};
    char *pToken;
    char txtcontents[50];
	
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len, i;

	struct sockaddr_in serv_adr,clnt_adr;
	socklen_t clnt_adr_sz;

	if(argc!=2){
		printf("Usage : %s <port> \n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock ==-1)
		error_handling("socket() error");
      
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	int opt = 1;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 1)==-1)
		error_handling("listen() error");

	clnt_adr_sz=sizeof(clnt_adr);

	//for(i=0;i<5 ; i++)
	//{
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);


		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
		{	

			//< #1. MCU에서 보내는 데이터 파싱 > (상태):(거리):(온도)L
			//write(clnt_sock, message, str_len); // 받은 데이터 확인
			message[str_len-1] = '\0';
			//write(clnt_sock, message, str_len); // 끝에 L-> NULL 변환 확인
			pToken = strtok(message, ":");	
			int i=0;
			while(pToken !=NULL)
			{
				pArray[i] = pToken;
				if(++i>4)
					break;
				pToken = strtok(NULL,":");
			}
			sprintf(state,pArray[2]);
			temp = atoi(state);
			sprintf(state,pArray[1]);
			level = atoi(state);
			sprintf(state,pArray[0]);
			//fputs(state,stdout);			// 값 확인완료
			//sprintf(state,pArray[1]);
			//sprintf(state,pArray[2]);		// 값은  정상적으로 파싱된 것으로 확인, 변환과정에서 문제가 발생
			//temp = atoi(pArray[1]);		
			//level = atoi(pArray[2]);		// 현재 여기에서 세그멘테이션 오류 발생중
			//str_len=strlen(state);
			//write(clnt_sock,state,str_len);	// 분리된 데이터 확인
			

			//< #2. DB에 파싱된 데이터 삽입 >
			if((!strcmp(state, "ON"))&&(level <=40))
			{
					sprintf(in_sql, "UPDATE water set WATERLEVEL = %d", 40-level);
					res = mysql_query(conn, in_sql);
					if(!res)
					{
						fputs("update water row\n",stdout);
					}
					else
					{
						error_handling("mysql update error\n");
					}

					sprintf(in_sql, "INSERT into temper(TIME, TEMPERATURE) values (curtime(), %d)",temp);
					res = mysql_query(conn, in_sql);
					if(!res)
					{
						fputs("inserted rows\n",stdout);
					}
					else
					{
						error_handling("mysql intsert error\n");
					}
			}

			//< #3. txt 파일에 상태 데이터 저장 >
			FILE* fpw = fopen("state.txt","w");
			if((!strcmp(state,"ON")))
			{
				sprintf(txtcontents, "<font size=\"15px\" color=\"red\">%s</font>",state);
			}
			else if((!strcmp(state,"OFF")))
			{
				sprintf(txtcontents, "<font size=\"15px\" color=\"white\">%s</font>",state);
			}
			fprintf(fpw,txtcontents);
			fflush(fpw);
			fclose(fpw);
		
			// < #4. html에서 변경한 상태 데이터 MCU로 보내기 >
			//FILE *fpr = fopen("SW_state.txt","r");
			//fgets(msg_cur, 10, fpr);
			//if(strcmp(msg_cur,msg_pre))
			//{
			//	write(clnt_sock,msg_cur,sizeof(msg_cur));
			//	strcpy(msg_pre, msg_cur);
			//}
			//fclose(fpr);


		}	

		close(clnt_sock);
	//}
	close(serv_sock);
	fflush(stdout);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
