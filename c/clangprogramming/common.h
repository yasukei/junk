#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

//-----------------------------------------------------------------------------
__inline static pid_t __gettid(void)
{
	return (pid_t)syscall(SYS_gettid);
}

//-----------------------------------------------------------------------------
#define ERROR \
	do\
	{\
		int errnoSv = errno;\
		fprintf(stderr,\
				"ERROR: %s@%d, tid: [%d], errno: [%d] [%s]\n",\
				__FILE__,\
				__LINE__,\
				__gettid(),\
				errnoSv,\
				strerror(errnoSv)\
			   );\
	}\
	while(false)\

