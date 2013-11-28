#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "AsyncCommClient.h"

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
struct AsyncCommClient
{
	int numofConcurrentSend;
	AsyncCommClientCallbacks callbacks;
};

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

