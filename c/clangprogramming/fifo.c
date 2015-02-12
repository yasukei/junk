#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

#include "common.h"

//-----------------------------------------------------------------------------
static void* __writer(void* arg)
{
	const char* fifo_name = (const char*)arg;
	int fifo_in;
	const char* str = "Hello World\n";
	int result;

	fifo_in = open(fifo_name, O_WRONLY);
	if(fifo_in == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	result = write(fifo_in, str, strlen(str) + 1);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	result = close(fifo_in);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
static void* __reader(void* arg)
{
	const char* fifo_name = (const char*)arg;
	int fifo_out;
	char buf[256];
	int result;

	fifo_out = open(fifo_name, O_RDONLY);
	if(fifo_out == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	result = read(fifo_out, buf, sizeof(buf));
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	printf("%s", buf);

	result = close(fifo_out);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
int main(void)
{
	const char* fifo_name = "/tmp/myfifo";
	int result;
	pthread_t writer, reader;

	result = mkfifo(fifo_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	pthread_create(&writer, NULL, __writer, (void*)fifo_name);
	pthread_create(&reader, NULL, __reader, (void*)fifo_name);

	pthread_join(writer, NULL);
	pthread_join(reader, NULL);

	result = unlink(fifo_name);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
