#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>//

static char * host = "localhost";//
static char * user = "root";//
static char * pass = "kcci";//
static char *dbname = "test";//

char device[] = "/dev/ttyACM0";
int fd;
unsigned long baud = 9600;

int main()
{
	MYSQL *conn;//
	conn = mysql_init(NULL);//
	int sql_index, flag =0;//
	char in_sql[200] = {0};//
	int res = 0;//

	if(!(mysql_real_connect(conn, host, user, pass, dbname, 0,NULL,0)))//
	{
		fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful! \n\n");


	char ser_buff[10]={0};
	int index = 0, temp, humi, str_len;
	char *pArray[2] = {0};
	char *pToken;
	printf("Raspberry Startup");
	fflush(stdout);
	if((fd= serialOpen(device, baud))<0){
		fprintf(stderr, "Unable %s\n", strerror(errno));
		exit(1);
	}
	if(wiringPiSetup() == -1)
		return 1;
	while(1)
	{
		if(serialDataAvail(fd))
		{
			ser_buff[index++] = serialGetchar(fd);
			if(ser_buff[index-1] == 'L')
			{
				flag =1;

				ser_buff[index-1] = '\0';
				str_len = strlen(ser_buff);
				//printf("ser_buff = %s\n", ser_buff);
				pToken = strtok(ser_buff,":");
				int i = 0;
				while(pToken != NULL)
				{
					pArray[i] = pToken;
					if(++i>3)
						break;
					pToken = strtok(NULL," ");
				}
				temp = atoi(pArray[0]);
				humi = atoi(pArray[1]);
				printf("temp = %d, humi = %d\n", temp, humi);
				for (int i=0 ; i<=str_len;i++)
					ser_buff[i] = 0;
				index = 0;
			}

			if(temp <100 && humi <100)//
			{
				if(flag == 1)
				{
					sprintf(in_sql, "insert into sensing(ID, DATE, TIME, MOISTURE, TEMPERATURE) values (null, curdate(), curtime(), %d,%d)",humi, temp);
					res = mysql_query(conn, in_sql);
					if(!res)
					{
						//printf("inserted %lu rows\n",(unsigned long) mysql_affected_rows(conn));
					}
					else
					{
						fprintf(stderr, "error : %s[%d]\n",mysql_error(conn), mysql_errno(conn));
						exit(1);
					}
				}
			}
		}
		flag = 0; //
	}fflush(stdout);

	return  0;
}
