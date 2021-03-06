#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"

//-----------------------------------------------------------------------------
int main(void)
{
	int pipefd[2];
	const char* str = "Hello World\n";
	char buf[256];
	int result;

	result = pipe(pipefd);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	result = write(pipefd[1], str, strlen(str) + 1);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	result = read(pipefd[0], buf, sizeof(buf));
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	printf("%s", buf);

	result = close(pipefd[0]);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}
	result = close(pipefd[1]);
	if(result == -1)
	{
		ERROR;
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
