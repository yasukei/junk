#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>
#include <pthread.h>

static void signalHandler_(
	int signum,
	siginfo_t* info,
	void* ucontext
	)
{
	printf("--- signal handler ---\n");
	printf("thread id:          [0x%08x]\n", (unsigned int)pthread_self());
	printf("signum:             [%d]\n", signum);
	printf("siginfo_t.si_signo: [%d]\n", info->si_signo);
	printf("siginfo_t.si_errno: [%d]\n", info->si_errno);
	printf("siginfo_t.si_code:  [%d]\n", info->si_code);
	printf("&signum:            [%p]\n", &signum);
	printf("&info:              [%p]\n", &info);
	printf("&ucontext:          [%p]\n", &ucontext);
	printf("\n");
}

static void* run_(
	void* arg
	)
{
	pthread_mutex_t* mutex = (pthread_mutex_t*)arg;
	pthread_attr_t attr;
	void* stack_addr;
	size_t stack_size;

	pthread_getattr_np(pthread_self(), &attr);
	pthread_attr_getstack(&attr, &stack_addr, &stack_size);

	pthread_mutex_lock(mutex);

	printf("--- pthread ---\n");
	printf("thread id:          [0x%08x]\n", (unsigned int)pthread_self());
	printf("stack addr:         [%p]\n", stack_addr);
	printf("stack size:         [0x%08x]\n", stack_size);
	printf("&arg:               [%p]\n", &arg);
	printf("\n");
	raise(SIGINT);
	raise(SIGINT);

	pthread_mutex_unlock(mutex);

	return NULL;
}

int main(void)
{
	struct sigaction action;
	pthread_t thread1;
	pthread_t thread2;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	action.sa_sigaction = signalHandler_;
	sigfillset(&(action.sa_mask));
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &action, NULL);

	printf("--- main ---\n");
	printf("thread id:          [0x%08x]\n", (unsigned int)pthread_self());
	printf("&action:            [%p]\n", &action);
	printf("&thread1:           [%p]\n", &thread1);
	printf("&thread2:           [%p]\n", &thread2);
	printf("\n");

	raise(SIGINT);
	raise(SIGINT);

	pthread_mutex_lock(&mutex);
	pthread_create(&thread1, NULL, run_, &mutex);
	pthread_create(&thread2, NULL, run_, &mutex);
	pthread_mutex_unlock(&mutex);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	return 0;
}

