#include <gmock/gmock.h>

#include "mock_classA.hpp"

extern "C" {

// ----------------------------------------------
void ClassA_doWork(ClassA* self, int money)
{
	((MockClassA*)self)->doWork(money);
}

// ----------------------------------------------
int ClassA_getProgress(ClassA* self)
{
	return ((MockClassA*)self)->getProgress();
}

}
