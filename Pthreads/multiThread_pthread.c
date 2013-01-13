#include <stdio.h>
#include <pthread.h>

#define NUMOF_THREADS 10

void* printHello(void* arg)
{
	printf("hello%d\n", *(int*)arg);
}

int main(void)
{
	int i;
	pthread_t threads[NUMOF_THREADS];
	int threadArg[NUMOF_THREADS];
	
	for(i = 0; i < NUMOF_THREADS; i++)
	{
		threadArg[i] = i;
		pthread_create(&threads[i], NULL, printHello, (void*)&threadArg[i]);
	}
	
	for(i = 0; i < NUMOF_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}
