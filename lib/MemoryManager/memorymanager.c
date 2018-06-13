
#define _GNU_SOURCE
#include <dlfcn.h>

#include <stdio.h>

#define BOOL	unsigned char
#define TRUE	1
#define FALSE	0

typedef struct MemoryManager
{
	void* (*realMalloc)(size_t);
	void* (*realCalloc)(size_t, size_t);
	void* (*realRealloc)(void*, size_t);
	void (*realFree)(void*);

	size_t counter;
}MemoryManager;

static MemoryManager gMemoryManager =
{
	NULL,
	NULL,
	NULL,
	NULL,
	0
};

static MemoryManager* __MemoryManager_getInstance(
	void
	)
{
	return &gMemoryManager;
}

static BOOL __MemoryManager_isInitialized(
	MemoryManager* self
	)
{
	return self->realMalloc != NULL;
}

static MemoryManager* __MemoryManager_initialize(
	MemoryManager* self
	)
{
	self->realMalloc = dlsym(RTLD_NEXT, "malloc");
	self->realCalloc = dlsym(RTLD_NEXT, "calloc");
	self->realRealloc = dlsym(RTLD_NEXT, "realloc");
	self->realFree = dlsym(RTLD_NEXT, "free");

	if(self->realMalloc == NULL
			|| self->realCalloc == NULL
			|| self->realRealloc == NULL
			|| self->realFree == NULL)
	{
		// error
		return NULL;
	}
	return self;
}

#define mMemoryManager_getInitializedInstance() \
	__MemoryManager_isInitialized(__MemoryManager_getInstance()) \
	? __MemoryManager_getInstance() \
	: __MemoryManager_initialize(__MemoryManager_getInstance());

static void* MemoryManager_malloc(
	size_t size
	)
{
	MemoryManager* self = mMemoryManager_getInitializedInstance();
	void* ptr = self->realMalloc(size);

	fprintf(stderr, "%p = malloc(%lu)\n", ptr, size);

	return ptr;
}

static void* MemoryManager_calloc(
	size_t nmemb,
	size_t size
	)
{
	MemoryManager* self = mMemoryManager_getInitializedInstance();
	void* ptr = self->realCalloc(nmemb, size);

	fprintf(stderr, "%p = calloc(%lu, %lu)\n", ptr, nmemb, size);

	return ptr;
}

static void* MemoryManager_realloc(
	void* ptr,
	size_t size
	)
{
	MemoryManager* self = mMemoryManager_getInitializedInstance();
	void* newPtr = self->realRealloc(ptr, size);

	fprintf(stderr, "%p = realloc(%p, %lu)\n", newPtr, ptr, size);

	return newPtr;
}

static void MemoryManager_free(
	void* ptr
	)
{
	MemoryManager* self = mMemoryManager_getInitializedInstance();

	fprintf(stderr, "free(%p)\n", ptr);

	return self->realFree(ptr);
}

void* malloc(
	size_t size
	)
{
	return MemoryManager_malloc(size);
}

void* calloc(
	size_t nmemb,
	size_t size
	)
{
	return MemoryManager_calloc(nmemb, size);
}

void* realloc(
	void* ptr,
	size_t size
	)
{
	return MemoryManager_realloc(ptr, size);
}

void free(
	void* ptr
	)
{
	return MemoryManager_free(ptr);
}

