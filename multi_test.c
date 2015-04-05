#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

/* This is our thread function.  It is like main(), but for a thread */
void *threadFunc(void *arg)
{
	char *str;
	int i = 0;

	str=(char*)arg;

	while(i < 10 )
	{
		usleep(rand() %400000);
		printf("threadFunc says: %s\n",str);
		++i;
	}

	return NULL;
}

int main(void)
{
	char * test = "www.mcgill.ca";
	pthread_t pth1, pth2;	// this is our thread identifier
	int i = 0;

	/* Create worker thread */
	pthread_create(&pth1,NULL,threadFunc,"processing...1");

	/* Create worker thread */
	pthread_create(&pth2,NULL,threadFunc,"processing...2");

	/* wait for our thread to finish before continuing */
	pthread_join(pth1, NULL /* void ** return value could go here */);

	/* wait for our thread to finish before continuing */
	pthread_join(pth2, NULL /* void ** return value could go here */);

	printf("size = %d\n",(int)(strlen(test)) );

	while(i < 10 )
	{
		sleep(1);
		printf("main() is running...\n");
		++i;
	}

	return 0;
}