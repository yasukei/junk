#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <signal.h>

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

int main(
		void
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

