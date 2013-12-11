#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "AsyncCommClient.h"
#include "private.h"

// ------------------------------------------------------------------
// define
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// enum
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// struct
// ------------------------------------------------------------------
struct AsyncCommClientCallbacks
{
	AsyncCommClientNormalCallback normalCallback;
};

// ------------------------------------------------------------------
typedef struct ACCService ACCService;
struct ACCService
{
};

// ------------------------------------------------------------------
struct AsyncCommClient
{
	int numofConcurrentSend;
	int socket;
	AsyncCommClientCallbacks callbacks;
};

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------
void __printErrnoDetails(int errnoSv)
{
	printf("errno: [%d], strerror: [%s]\n", errnoSv, strerror(errnoSv));
}

// ------------------------------------------------------------------
// AsyncCommClientCallbacks
// ------------------------------------------------------------------
static void __AsyncCommClientCallbacks_initialize(
	AsyncCommClientCallbacks* self
	)
{
	self->normalCallback = NULL;
}

// ------------------------------------------------------------------
AsyncCommClientCallbacks* AsyncCommClientCallbacks_create(void)
{
	AsyncCommClientCallbacks* self;

	self = malloc(sizeof(AsyncCommClientCallbacks));
	if(self != NULL)
	{
		__AsyncCommClientCallbacks_initialize(self);
	}

	return self;
}

// ------------------------------------------------------------------
void AsyncCommClientCallbacks_delete(
	AsyncCommClientCallbacks* self
	)
{
	if(self != NULL)
	{
		free(self);
	}
}

// ------------------------------------------------------------------
void AsyncCommClientCallbacks_setNormalCallback(
	AsyncCommClientCallbacks* self,
	AsyncCommClientNormalCallback callback
	)
{
	if(self != NULL)
	{
		self->normalCallback = callback;
	}
}

// ------------------------------------------------------------------
// AsyncCommClient
// ------------------------------------------------------------------
bool __AsyncCommClient_initialize(
	AsyncCommClient* self
	)
{
	(void)self;
	return true;
}

// ------------------------------------------------------------------
AsyncCommClient* AsyncCommClient_create(
	int numofConcurrentSend
	)
{
	AsyncCommClient* self;

	if(numofConcurrentSend <= 0)
	{
		goto lError;
	}

	self = malloc(sizeof(AsyncCommClient));
	if(self == NULL)
	{
		goto lError;
	}
	if(! __AsyncCommClient_initialize(self))
	{
		goto lError;
	}
	return self;

lError:
	return NULL;
}

// ------------------------------------------------------------------
void AsyncCommClient_delete(
	AsyncCommClient* self
	)
{
	if(self == NULL)
	{
		return;
	}

	free(self);
}

// ------------------------------------------------------------------
bool AsyncCommClient_connectServer(
	AsyncCommClient* self,
	int family,
	const char* serverName,
	uint16_t port,
	const char* unixDomainName
	)
{
	if(self == NULL)
	{
		DEBUGPOINT;
		return false;
	}

	struct sockaddr_in serverAddr;

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverName);

	self->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(self->socket == -1)
	{
		__printErrnoDetails(errno); DEBUGPOINT;
		return false;
	}

	if(connect(self->socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
	{
		__printErrnoDetails(errno); DEBUGPOINT;
		return false;
	}

	return true;
}

// ------------------------------------------------------------------
void AsyncCommClient_disconnectServer(AsyncCommClient* self)
{
	close(self->socket);
}

// ------------------------------------------------------------------
bool AsyncCommClient_send(
	AsyncCommClient* self,
	AsyncCommClientCallbacks* callbacks,
	unsigned char* sendData,
	size_t sizeofSendData,
	uint32_t* sendId
	)
{
	return true;
}
