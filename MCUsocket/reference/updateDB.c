#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

static char * host = "localhost";
static char * user = "root";
static char * pass = "kcci";
static char * dbname = "RaspPJ";

char device[]="/dev/ttyACM0";
int fd;
unsigned long baud = 115200;

int main()
{
	MYSQL * conn;
	conn = mysql_init(NULL);
	int sql_index, flag=0;
	char in_sql[200] = {0};
	int res = 0;

	if(!(mysql_real_connect(conn, host, user, pass, dbname, 0,NULL,0)))
	{
		fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	//else
		//printf("Connection Successful! \n\n");

	char ser_buff[30] = {0};
	int index = 0, temp, level, str_len;
	char state[10];
	char msg_pre[10]={0};
	char msg_cur[10]={0};
	char *pArray[3] = {0};
	char *pToken;
	char txtcontents[20];
	//printf("Raspberry Startup");
	fflush(stdout);
	if((fd=serialOpen(device,baud))<0){
		fprintf(stderr, "Unable %s\n", strerror(errno));
		exit(1);
	}
	if(wiringPiSetup()==-1)
		return 1;

	while(1)
	{
		if(serialDataAvail(fd))
		{
			ser_buff[index++] = serialGetchar(fd);

			if(ser_buff[index-1] == 'L')
			{
				flag = 1;

				ser_buff[index -1] = '\0';
				str_len = strlen(ser_buff);
				//printf("ser_buff = %s\n",ser_buff);
				pToken = strtok(ser_buff,":");
				int i=0;
				while(pToken !=NULL)
				{
					pArray[i] = pToken;
					if(++i>4)
						break;
					pToken = strtok(NULL,":");
				}
				sprintf(state,pArray[0]);
				temp = atoi(pArray[1]);
				level = atoi(pArray[2]);
				//printf("state = %s, temp = %d, level = %d\n",state,temp,level);
				for(int i=0;i<=str_len;i++)
					ser_buff[i] = 0;
				index = 0;
			}

			if( (!strcmp(state,"ON"))&&(level <= 40))
			{
				if(flag == 1)
				{
					sprintf(in_sql, "UPDATE water set WATERLEVEL = %d",40-level);
					res = mysql_query(conn, in_sql);
					sprintf(in_sql, "INSERT into temper(TIME, TEMPERATURE) values (curtime(), %d)",temp);
					res = mysql_query(conn, in_sql);

					if(!res)
					{
						//printf("inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn));
					}
					else
					{
						fprintf(stderr,"error : %s[%d]\n",mysql_error(conn), mysql_errno(conn));
						exit(1);
					}
				}
			}

			FILE* fpw = fopen("state.txt","w"); // 쓰기모드로 state.txt 파일을 열음
			if((!strcmp(state,"ON")))
			{
				sprintf(txtcontents,"<font size=\"15px\" color=\"red\">%s</font>",state);
			}
			else if((!strcmp(state,"OFF")))
			{
				sprintf(txtcontents,"<font size=\"15px\" color=\"white\">%s</font>",state);
			}
			fprintf(fpw,txtcontents);
			fflush(fpw);
			fclose(fpw);


			FILE* fpr = fopen("SW_state.txt","r");
			fgets(msg_cur, 10, fpr);
			if(strcmp(msg_cur,msg_pre))
			{
				printf("%s",msg_cur);
				strcpy(msg_pre,msg_cur);
			}
			fclose(fpr);



		}flag = 0;
	}fflush(stdout);

	return 0;
}
