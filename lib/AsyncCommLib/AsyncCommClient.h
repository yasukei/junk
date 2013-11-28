#ifndef ASYNCCOMMCLIENT_H_
#define ASYNCCOMMCLIENT_H_

// ------------------------------------------------------------------
// enum
// ------------------------------------------------------------------
enum eAsyncCommClientResult
{
	eAsyncCommClientResult_success,
};

// ------------------------------------------------------------------
// struct
// ------------------------------------------------------------------
typedef struct AsyncCommClientCallbacks AsyncCommClientCallbacks;
typedef struct AsyncCommClient AsyncCommClient;

// ------------------------------------------------------------------
// AsyncCommClientCallbacks I/F
// ------------------------------------------------------------------
typedef void (*AsyncCommClientNormalCallback)(int result);

AsyncCommClientCallbacks* AsyncCommClientCallbacks_create(void);
void AsyncCommClientCallbacks_delete(AsyncCommClientCallbacks* self);
void AsyncCommClientCallbacks_setNormalCallback(
	AsyncCommClientCallbacks* self,
	AsyncCommClientNormalCallback callback
	);

// ------------------------------------------------------------------
// AsyncCommClient I/F
// ------------------------------------------------------------------
AsyncCommClient* AsyncCommClient_create(int numofConcurrentSend);
void AsyncCommClient_delete(AsyncCommClient* self);

#endif /* ASYNCCOMMCLIENT_H_ */
