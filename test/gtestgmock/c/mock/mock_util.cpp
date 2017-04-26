#include <gmock/gmock.h>

#include "mock_util.hpp"

MockUtil* gInstance;

void MockUtil::setInstance(MockUtil* instance)
{
	gInstance = instance;
}

extern "C" {

// ----------------------------------------------
void* UtilMalloc(size_t size)
{
	return gInstance->UtilMalloc(size);
}

// ----------------------------------------------
void UtilFree(void* ptr)
{
	gInstance->UtilFree(ptr);
}

}
