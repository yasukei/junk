#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <signal.h>

#define CHECKPOINT printf("CP: %s:%d\n", __FILE__, __LINE__);
#define CHECK_POINT(fmt, ...) printf("CP: %s:%d, " fmt, __FILE__, __LINE__, __VA_ARGS__);

static void __sigAction(
		int signum,
		siginfo_t* info,
		void* ucontext
		)
{
	printf("signalHandler_tid: [%d]\n", syscall(SYS_gettid));
	printf("signum: [%d]\n", signum);
	printf("siginfo_t.si_signo:\t[%d]\n", info->si_signo);
	printf("siginfo_t.si_errno:\t[%d]\n", info->si_errno);
	printf("siginfo_t.si_code:\t[%d]\n", info->si_code);

	exit(1);
}

void __signal(
		void* argv[]
		)
{
	struct sigaction action;

	action.sa_sigaction = __sigAction;
	sigfillset(&(action.sa_mask));
	action.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &action, NULL);

	printf("main_pid: [%d]\n", syscall(SYS_getpid));
	printf("main_tid: [%d]\n", syscall(SYS_gettid));

	while(1)
	{
		sleep(1);
	}
}

void __memset(
		void* argv[]
		)
{
	CHECK_POINT("dst is int[]");
	{
		int dst[32];
		int c = 512;
		size_t n = 8;

		//memset(buf, 512, 
	}
}

struct FunctionTable
{
	char* functionName;
	void (*functionPointer)(void* argv[]);
};

struct FunctionTable gFunctionTable[] =
{
	{ "signal", __signal },
	{ "memset", __memset },
};

int main(
		int argc,
		void* argv[]
		)
{
	int i;

	if(argc == 1)
	{
		printf("usage: behavior [function name]\n");
		return 0;
	}

	printf("arrayLength: [%lu]\n", sizeof(gFunctionTable) / sizeof(struct FunctionTable));

	for(i = 0; i < (sizeof(gFunctionTable) / sizeof(struct FunctionTable)); i++)
	{
		if(strcmp(argv[1], gFunctionTable[i].functionName) == 0)
		{
			gFunctionTable[i].functionPointer(argv);
			break;
		}
	}

	return 0;
}

