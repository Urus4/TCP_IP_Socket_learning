#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
	pthread_t t_id1;
	pthread_t t_id2;
	int thread_param=5;
	void * thr_ret1;
	void * thr_ret2;

	if(pthread_create(&t_id1, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread1_create() error");
		return -1;
	};

	if(pthread_create(&t_id2, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread2_create() error");
		return -1;

	}


	if(pthread_join(t_id1, &thr_ret1)!=0)
	{
		puts("pthread1_join() error");
		return -1;
	}

	if(pthread_join(t_id2, &thr_ret2)!=0)
	{
		puts("pthread2_join() error");
		return -1;
	}

	printf("Thread1 return message : %s \n", (char*)thr_ret1);
	printf("Thread2 return message : %s \n", (char*)thr_ret2);
	free(thr_ret1);
	free(thr_ret2);
	return 0;
}

void* thread_main(void *arg)
{
	int i;
	int cnt = *((int*)arg);
	char *msg=(char*)malloc(sizeof(char)*50);
	strcpy(msg, "Hello, I'm thread~ \n");

	for(i=0; i<cnt; i++)
	{
		sleep(1);
		puts("running thread");
	}
	return (void*)msg;
}
