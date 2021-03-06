#include <mqueue.h>
#include <fcntl.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "common.h"

//-----------------------------------------------------------------------------
int main(void)
{
	const char* mq_name = "/mymq";
	struct mq_attr mqattr;
	mqd_t mq;
	const char* str = "Hello World\n";
	int prio = 0;
	char buf[256];
	int result;

	mqattr.mq_flags = 0;
	mqattr.mq_maxmsg = 1;
	mqattr.mq_msgsize = sizeof(buf);

	result = mq_unlink(mq_name);
	mq = mq_open(
			mq_name,
			O_CREAT | O_EXCL | O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
			&mqattr
			);
	if(mq == -1)
	{
		ERROR;
		return -1;
	}

	result = mq_send(mq, str, strlen(str) + 1, prio);
	if(result == -1)
	{
		ERROR;
		return -1;
	}

	result = mq_receive(mq, buf, sizeof(buf), NULL);
	if(result == -1)
	{
		ERROR;
		return -1;
	}

	printf("%s", buf);

	result = mq_close(mq);
	if(result == -1)
	{
		ERROR;
		return -1;
	}

	result = mq_unlink(mq_name);
	if(result == -1)
	{
		ERROR;
		return -1;
	}

	return 0;
}
