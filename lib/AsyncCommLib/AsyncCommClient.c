#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

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
// prototype
// ------------------------------------------------------------------
typedef struct ACCNormalCallback ACCNormalCallback;
typedef struct ACCRequest ACCRequest;
typedef struct ACCRequestList ACCRequestList;
typedef struct ACCService ACCService;
typedef void (*ACCServiceRoutine)(void* context);

// ------------------------------------------------------------------
// struct
// ------------------------------------------------------------------
struct ACCNormalCallback
{
	AsyncCommClientNormalCallback callback;
	void* context;
};

// ------------------------------------------------------------------
struct AsyncCommClientConfig
{
	uint32_t numofConcurrentSend;
};

// ------------------------------------------------------------------
struct AsyncCommClientCallbacks
{
	ACCNormalCallback normal;
};

// ------------------------------------------------------------------
struct ACCServerInformation
{
	int family;
	union
	{
		struct
		{
			char* serverName;
			uint16_t port;
		}inet;

		struct
		{
			char* unixDomainName;
		}unix_domain;

	}family_attr;
};

//// ------------------------------------------------------------------
//typedef struct ACCConnection
//{
//	int socket;
//}ACCConnection;

// ------------------------------------------------------------------
struct ACCPacket
{
	uint32_t length;
	uint32_t sendId;
	uint32_t reserved;
	unsigned char* userData;
};

// ------------------------------------------------------------------
struct ACCRequest
{
	uint32_t sendId;
	AsyncCommClientCallbacks callbacks;

	ACCRequest* next; // for ACCRequestList
};

// ------------------------------------------------------------------
typedef struct ACCSynchronizer
{
	pthread_mutex_t mutex;
	pthread_mutex_t cond;
	bool isSynchronized;
}ACCSynchronizer;

// ------------------------------------------------------------------
struct ACCRequestList
{
	pthread_mutex_t mutex;
	ACCRequest* head;
	ACCRequest* tail;
	uint32_t numofRequests;
};

// ------------------------------------------------------------------
typedef struct ACCRequestPool
{
	ACCRequestList requestList;
	ACCRequest* requests;
	uint32_t numofAllocatedRequests;
}ACCRequestPool;

// ------------------------------------------------------------------
typedef struct ACCServiceRecipe
{
	ACCServiceRoutine routine;
	ACCRequestList* input;
	ACCRequestList* output;
}ACCServiceRecipe;

// ------------------------------------------------------------------
struct ACCService
{
	pthread_t thread;
	bool stopRequested;
	ACCServiceRecipe recipe;
};

// ------------------------------------------------------------------
struct AsyncCommClient
{
	ACCRequestPool pool;
	int socket;
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
void AsyncCommClientCallbacks_setNormalCallback(
	AsyncCommClientCallbacks* self,
	AsyncCommClientNormalCallback callback,
	void* context
	)
{
	if(self != NULL)
	{
		self->normal.callback = callback;
		self->normal.context = context;
	}
}

// ------------------------------------------------------------------
__inline void AsyncCommClientCallbacks_resetNormalCallback(
	AsyncCommClientCallbacks* self
	)
{
	AsyncCommClientCallbacks_setNormalCallback(self, NULL, NULL);
}

// ------------------------------------------------------------------
static void __AsyncCommClientCallbacks_initialize(
	AsyncCommClientCallbacks* self
	)
{
	AsyncCommClientCallbacks_resetNormalCallback(self);
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
void AsyncCommClientCallbacks_execNormalCallback(
	AsyncCommClientCallbacks* self,
	uint32_t sendId,
	unsigned char* receiveData,
	size_t sizeofReceiveData,
	int result
	)
{
	if(self->normal.callback != NULL)
	{
		self->normal.callback(
			self->normal.context,
			sendId,
			receiveData,
			sizeofReceiveData,
			result
			);
	}
}

// ------------------------------------------------------------------
// AsyncCommClientConfig
// ------------------------------------------------------------------
AsyncCommClientConfig* AsyncCommClientConfig_create(
	void
	)
{
	AsyncCommClientConfig* self;

	self = malloc(sizeof(AsyncCommClientConfig));
	if(self != NULL)
	{
		self->numofConcurrentSend = 32;
	}

	return self;
}

// ------------------------------------------------------------------
void AsyncCommClientConfig_delete(
	AsyncCommClientConfig* self
	)
{
	if(self != NULL)
	{
		self->numofConcurrentSend = 0;
		free(self);
	}
}

// ------------------------------------------------------------------
bool AsyncCommClientConfig_setNumofConcurrentSend(
	AsyncCommClientConfig* self,
	uint32_t numofConcurrentSend
	)
{
	if(self != NULL)
	{
		self->numofConcurrentSend = numofConcurrentSend;
		return true;
	}
	return false;
}

// ------------------------------------------------------------------
uint32_t AsyncCommClientConfig_getNumofConcurrentSend(
	AsyncCommClientConfig* self
	)
{
	if(self != NULL)
	{
		return self->numofConcurrentSend;
	}
	return 0;
}

// ------------------------------------------------------------------
bool AsyncCommClientConfig_isValid(
	AsyncCommClientConfig* self
	)
{
	if( (self == NULL)
		|| (self->numofConcurrentSend == 0)
	  )
	{
		return false;
	}
	return true;
}

//// ------------------------------------------------------------------
//// ACCConnection
//// ------------------------------------------------------------------
//bool ACCConnection_initialize(
//	ACCConnection* self
//	)
//{
//	self->socket = 0;
//}
//
//// ------------------------------------------------------------------
//void ACCConnection_finalize(
//	ACCConnection* self
//	)
//{
//	self->socket = 0;
//}
//
//// ------------------------------------------------------------------
//bool ACCConnection_open(
//	ACCConnection* self
//	)
//{
//	(void)self;
//}
//
//// ------------------------------------------------------------------
//bool ACCConnection_close(
//	ACCConnection* self
//	)
//{
//	(void)self;
//}
//
//// ------------------------------------------------------------------
//bool ACCConnection_send(
//	ACCConnection* self,
//	unsigned char* sendData,
//	size_t sizeofSendData,
//	size_t* actualSizeofSentData
//	)
//{
//	(void)self;
//	(void)sendData;
//	(void)sizeofSendData;
//	(void)actualSizeofSentData;
//}
//
//// ------------------------------------------------------------------
//bool ACCConnection_receive(
//	ACCConnection* self,
//	unsigned char* buffer,
//	size_t sizeofBuffer,
//	size_t* sizeofReceivedData
//	)
//{
//	(void)self;
//	(void)buffer;
//	(void)sizeofBuffer;
//	(void)sizeofReceivedData;
//}

// ------------------------------------------------------------------
// ACCRequest
// ------------------------------------------------------------------
bool ACCRequest_initialize(
	ACCRequest* self
	)
{
	self->sendId = 1;
	return true;
}

// ------------------------------------------------------------------
void ACCRequest_finalize(
	ACCRequest* self
	)
{
	self->sendId = 0;
}

// ------------------------------------------------------------------
void ACCRequest_setCallbacks(
	ACCRequest* self,
	AsyncCommClientCallbacks* callbacks
	)
{
	self->callbacks = *callbacks;
}

// ------------------------------------------------------------------
uint32_t ACCRequest_getSendId(
	ACCRequest* self
	)
{
	return self->sendId;
}

// ------------------------------------------------------------------
// ACCRequestList
// ------------------------------------------------------------------
bool ACCRequestList_initialize(
	ACCRequestList* self
	)
{
	self->head = NULL;
	self->tail = NULL;
	self->numofRequests = 0;
	if(pthread_mutex_init(&(self->mutex), NULL) != 0)
	{
		return false;
	}
	return true;
}

// ------------------------------------------------------------------
void ACCRequestList_finalize(
	ACCRequestList* self
	)
{
	(void)pthread_mutex_destroy(&(self->mutex));
	self->numofRequests = 0;
	self->tail = NULL;
	self->head = NULL;
}

// ------------------------------------------------------------------
static void __ACCRequestList_lock(
	ACCRequestList* self
	)
{
	(void)pthread_mutex_lock(&(self->mutex));
}

// ------------------------------------------------------------------
static void __ACCRequestList_unlock(
	ACCRequestList* self
	)
{
	(void)pthread_mutex_unlock(&(self->mutex));
}

// ------------------------------------------------------------------
void ACCRequestList_enqueue(
	ACCRequestList* self,
	ACCRequest* request
	)
{
	request->next = NULL;

	__ACCRequestList_lock(self);
	if(self->numofRequests == 0)
	{
		self->numofRequests = 1;
		self->head = request;
		self->tail = request;
	}
	else
	{
		self->numofRequests += 1;
		self->tail->next = request;
		self->tail = request;
	}
	__ACCRequestList_unlock(self);
}

// ------------------------------------------------------------------
static ACCRequest* __ACCRequestList_dequeue(
	ACCRequestList* self
	)
{
	ACCRequest* request;

	if(self->numofRequests >= 2)
	{
		self->numofRequests -= 1;
		request = self->head;
		self->head = request->next;
	}
	else if(self->numofRequests == 1)
	{
		self->numofRequests = 0;
		request = self->head;
		self->head = NULL;
		self->tail = NULL;
	}
	else
	{
		request = NULL;
	}

	request->next = NULL;
	return request;
}

// ------------------------------------------------------------------
ACCRequest* ACCRequestList_dequeue(
	ACCRequestList* self
	)
{
	ACCRequest* request;

	__ACCRequestList_lock(self);
	request = __ACCRequestList_dequeue(self);
	__ACCRequestList_unlock(self);

	return request;
}

// ------------------------------------------------------------------
// ACCRequestPool
// ------------------------------------------------------------------
bool ACCRequestPool_initialize(
	ACCRequestPool* self,
	AsyncCommClientConfig* config
	)
{
	uint32_t index;

	self->numofAllocatedRequests = AsyncCommClientConfig_getNumofConcurrentSend(config);
	if(! ACCRequestList_initialize(&(self->requestList)))
	{
		goto lError0;
	}
	self->requests = malloc(sizeof(ACCRequest) * self->numofAllocatedRequests);
	if(self->requests == NULL)
	{
		goto lError1;
	}
	for(index = 0; index < self->numofAllocatedRequests; index++)
	{
		if(! ACCRequest_initialize(&(self->requests[index])))
		{
			goto lError2;
		}
	}
	for(index = 0; index < self->numofAllocatedRequests; index++)
	{
		ACCRequestList_enqueue(&(self->requestList), &(self->requests[index]));
	}

	return true;

lError2:
	for(; index > 0; index--)
	{
		ACCRequest_finalize(&(self->requests[index-1]));
	}
	free(self->requests);
lError1:
	ACCRequestList_finalize(&(self->requestList));
lError0:
	self->numofAllocatedRequests = 0;
	return false;
}

// ------------------------------------------------------------------
void ACCRequestPool_finalize(
	ACCRequestPool* self
	)
{
	uint32_t index;

	while(ACCRequestList_dequeue(&(self->requestList)) != NULL)
	{
		;
	}
	for(index = 0; index < self->numofAllocatedRequests; index++)
	{
		ACCRequest_finalize(&(self->requests[index]));
	}
	free(self->requests);
	ACCRequestList_finalize(&(self->requestList));
	self->numofAllocatedRequests = 0;
}

// ------------------------------------------------------------------
ACCRequest* ACCRequestPool_getNewRequest(
	ACCRequestPool* self
	)
{
	return ACCRequestList_dequeue(&(self->requestList));
}

// ------------------------------------------------------------------
void ACCRequestPool_returnRequest(
	ACCRequestPool* self,
	ACCRequest* request
	)
{
	//ACCRequest_cleanup(request);
	return ACCRequestList_enqueue(&(self->requestList), request);
}

//// ------------------------------------------------------------------
//// ACCService
//// ------------------------------------------------------------------
//bool ACCService_initialize(
//	ACCService* self
//	)
//{
//	self->stopRequested = false;
//	return true;
//}
//
//// ------------------------------------------------------------------
//void ACCService_finalize(
//	ACCService* self
//	)
//{
//	self->stopRequested = false;
//}
//
//// ------------------------------------------------------------------
//static void* __ACCService_run(
//	void* arg
//	)
//{
//	ACCService* self = (ACCService*)arg;
//
//	self->routine(self);
//	return NULL;
//}
//
//// ------------------------------------------------------------------
//bool ACCService_start(
//	ACCService* self,
//	ACCServiceProcedure procedure
//	)
//{
//	self->procedure = procedure;
//	self->stopRequested = false;
//	if(pthread_create(&self->thread, NULL, __ACCService_run, (void*)self) != 0)
//	{
//		return false;
//	}
//	return true;
//}
//
//// ------------------------------------------------------------------
//void ACCService_stop(
//	ACCService* self
//	)
//{
//	__ACCService_lock(self);
//	self->stopRequested = true;
//	pthread_cond_signal(&self->condvar);
//	__ACCService_unlock(self);
//	if(pthread_join(&self->thread, NULL) != 0)
//	{
//		__printErrnoDetails(errno);
//		CHECKPOINT;
//	}
//}
//
//// ------------------------------------------------------------------
//bool ACCService_isStopRequested(
//	ACCService* self
//	)
//{
//	return self->stopRequested;
//}
//
//// ------------------------------------------------------------------
//// ACCSendProcedure
//// ------------------------------------------------------------------
//static __inline void ACCSendProcedure_waitForRequest(
//	ACCRequestList* input
//	)
//{
//	ACCRequestList_waitForEnqueued(input);
//}
//
//// ------------------------------------------------------------------
//static __inline void ACCSendProcedure_consumeRequest(
//	ACCRequestList* input,
//	ACCRequestList* output
//	)
//{
//	ACCRequest* request;
//
//	request = ACCRequestList_dequeue(input);
//	if(request == NULL)
//	{
//		return;
//	}
//
//	
//}
//
//// ------------------------------------------------------------------
//void ACCSendProcedure_execute(
//	ACCService* service,
//	ACCRequestList* input,
//	ACCRequestList* output
//	)
//{
//	while(ACCService_isInService(service))
//	{
//		ACCSendProcedure_waitForRequest(input);
//		ACCSendProcedure_consumeRequest(input, output);
//	}
//}

// ------------------------------------------------------------------
// AsyncCommClient
// ------------------------------------------------------------------
static bool __AsyncCommClient_initialize(
	AsyncCommClient* self,
	AsyncCommClientConfig* config
	)
{
	if(! ACCRequestPool_initialize(&(self->pool), config))
	{
		goto lError0;
	}
	return true;

lError0:
	return false;
}

// ------------------------------------------------------------------
static void __AsyncCommClient_finalize(
	AsyncCommClient* self
	)
{
	ACCRequestPool_finalize(&(self->pool));
}

// ------------------------------------------------------------------
AsyncCommClient* AsyncCommClient_create(
	AsyncCommClientConfig* config
	)
{
	AsyncCommClient* self;

	if(! AsyncCommClientConfig_isValid(config))
	{
		goto lError1;
	}

	self = malloc(sizeof(AsyncCommClient));
	if(self == NULL)
	{
		goto lError2;
	}
	if(! __AsyncCommClient_initialize(self, config))
	{
		goto lError3;
	}
	return self;

lError3:
	free(self);
lError2:
lError1:
	return NULL;
}

// ------------------------------------------------------------------
void AsyncCommClient_delete(
	AsyncCommClient* self
	)
{
	if(self != NULL)
	{
		__AsyncCommClient_finalize(self);
		free(self);
	}
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

	(void)family;
	(void)unixDomainName;
	{
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
	}

	return true;
}

// ------------------------------------------------------------------
void AsyncCommClient_disconnectServer(AsyncCommClient* self)
{
	close(self->socket);
}

// ------------------------------------------------------------------
static __inline bool __AsyncCommClient_isInvalidArgsForSend(
	AsyncCommClient* self,
	AsyncCommClientCallbacks* callbacks,
	unsigned char* sendData,
	size_t sizeofSendData,
	uint32_t* sendId
	)
{
	if((self == NULL)
		|| (callbacks == NULL)
		|| (sendData == NULL)
		|| (sizeofSendData == 0)
		|| (sendId == NULL)
	  )
	{
		return false;
	}
	return true;
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
	ACCRequest* request;

	if(__AsyncCommClient_isInvalidArgsForSend(
				self,
				callbacks,
				sendData,
				sizeofSendData,
				sendId
				)
	  )
	{
		return false;
	}

	request = ACCRequestPool_getNewRequest(&(self->pool));
	if(request == NULL)
	{
		return false;
	}
	ACCRequest_setCallbacks(request, callbacks);
	*sendId = ACCRequest_getSendId(request);

	//ACCService_request(&(self->sendService));
	return true;
}

// ------------------------------------------------------------------
bool AsyncCommClient_cancel(
	AsyncCommClient* self,
	uint32_t sendId
	)
{
	(void)self;
	(void)sendId;
	return false;
}

