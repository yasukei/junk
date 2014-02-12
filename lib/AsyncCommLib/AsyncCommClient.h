#ifndef ASYNCCOMMCLIENT_H_
#define ASYNCCOMMCLIENT_H_

#include <stddef.h>
#include <stdint.h>

// ------------------------------------------------------------------
// enum
// ------------------------------------------------------------------
enum eAsyncCommClientAddressFamily
{
	eAsyncCommClientAddressFamily_INET,
	eAsyncCommClientAddressFamily_UNIX,
};

enum eAsyncCommClientResult
{
	eAsyncCommClientResult_success,
	eAsyncCommClientResult_unexpectedError,
};

// ------------------------------------------------------------------
// struct
// ------------------------------------------------------------------
typedef struct AsyncCommClientCallbacks AsyncCommClientCallbacks;
typedef struct AsyncCommClient AsyncCommClient;

// ------------------------------------------------------------------
// AsyncCommClientCallbacks I/F
// ------------------------------------------------------------------
typedef void (*AsyncCommClientNormalCallback)(
			void* context,
			uint32_t sendId,
			unsigned char* receiveData,
			size_t sizeofReceiveData,
			int result
			);

AsyncCommClientCallbacks* AsyncCommClientCallbacks_create(void);
void AsyncCommClientCallbacks_setNormalCallback(
	AsyncCommClientCallbacks* self,
	AsyncCommClientNormalCallback callback,
	void* context
	);
void AsyncCommClientCallbacks_delete(AsyncCommClientCallbacks* self);

// ------------------------------------------------------------------
// AsyncCommClient I/F
// ------------------------------------------------------------------
AsyncCommClient* AsyncCommClient_create(int numofConcurrentSend);
bool AsyncCommClient_connectServer(
	AsyncCommClient* self,
	int family,
	const char* serverName,
	uint16_t port,
	const char* unixDomainName
	);
bool AsyncCommClient_send(
	AsyncCommClient* self,
	AsyncCommClientCallbacks* callbacks,
	unsigned char* sendData,
	size_t sizeofSendData,
	uint32_t* sendId
	);
bool AsyncCommClient_cancel(AsyncCommClient* self, uint32_t* sendId);
void AsyncCommClient_disconnectServer(AsyncCommClient* self);
void AsyncCommClient_delete(AsyncCommClient* self);

#endif /* ASYNCCOMMCLIENT_H_ */
