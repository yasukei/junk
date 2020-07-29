#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#define CHECKPOINT fprintf(stdout, "CP: %d@%s\n", __LINE__, __FILE__)

//-------------------------------------------------------------------
typedef void (*SignalHandler)(int signum, siginfo_t* info, void* ucontext);

//-------------------------------------------------------------------
static void __registerSignalHandler(
	int signalNumber,
	SignalHandler handler
	)
{
	struct sigaction sa;

	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);
	sigaction(signalNumber, &sa, NULL);
}

//-------------------------------------------------------------------
static void __handleSignal(
	int signum,
	siginfo_t* info,
	void* ucontext
	)
{
	//printf("--- signal handler ---\n");
	//printf("thread id:          [0x%08x]\n", (unsigned int)pthread_self());
	//printf("signum:             [%d]\n", signum);
	//printf("siginfo_t.si_signo: [%d]\n", info->si_signo);
	//printf("siginfo_t.si_errno: [%d]\n", info->si_errno);
	//printf("siginfo_t.si_code:  [%d]\n", info->si_code);
	//printf("\n");
}

//-------------------------------------------------------------------
static void __printErrno(int errnoSv)
{
	printf("errno: [%d], strerror(errno): %s\n", errnoSv, strerror(errnoSv));
}

//-------------------------------------------------------------------
static void* __writer(
	void* arg
	)
{
	int* socket = (int*)arg;
	bool isRunning = true;
	uint8_t buf[4096];
	uint8_t data = 0;

	while(isRunning)
	{
		size_t length = sizeof(buf);
		size_t offset = 0;

		memset(buf, data, length);
		data++;

		while(true)
		{
			int result = write(*socket, &buf[offset], length);
			if(result < 0)
			{
				int errnoSv = errno;

				__printErrno(errnoSv);
				if(errnoSv == EAGAIN || errnoSv == EWOULDBLOCK || errnoSv == EINTR)
				{
				}
				else if(errnoSv == EPIPE || true)
				{
					CHECKPOINT;
					isRunning = false;
					break;
				}
			}
			else if(result < length)
			{
				offset += length - result;
			}
			else
			{
				break;
			}
		}
	}
	CHECKPOINT;

	return NULL;
}

//-------------------------------------------------------------------
static void* __reader(
	void* arg
	)
{
	int* socket = (int*)arg;
	bool isRunning = true;
	uint8_t buf[8];
	const size_t bufSize = sizeof(buf);

	while(isRunning)
	{
		int result = read(*socket, buf, bufSize);
		if(result < 0)
		{
			int errnoSv = errno;

			__printErrno(errnoSv);
			if(errnoSv == EAGAIN || errnoSv == EWOULDBLOCK || errnoSv == EINTR)
			{
				continue;
			}
			else if(errnoSv == EPIPE || true)
			{
				CHECKPOINT;
				isRunning = false;
			}
		}
		else
		{
			printf("read result: [%d], buf: [ ", result);
			for(int i = 0; i < result; i++)
			{
				printf("0x%X ", buf[i]);
			}
			printf("]\n");

			sleep(1);
		}
	}
	CHECKPOINT;

	return NULL;
}

//-------------------------------------------------------------------
int main(void)
{
	// set signal handler
	__registerSignalHandler(SIGUSR1, __handleSignal);

	// make socket pair
	int sockets[2];
	int result = socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
	if(result < 0)
	{
		int errnoSv = errno;
		CHECKPOINT;
		__printErrno(errnoSv);
		return -1;
	}

	pthread_t writer;
	pthread_t reader;
	pthread_create(&writer, NULL, __writer, &sockets[0]);
	pthread_create(&reader, NULL, __reader, &sockets[1]);
	
	sleep(3);
	for(uint64_t i = 0; i < 1000*1000; i++)
	{
		raise(SIGUSR1);
	}
	close(sockets[1]);
	close(sockets[0]);
	pthread_join(writer, NULL);
	pthread_join(reader, NULL);

	CHECKPOINT;
	return 0;
}

