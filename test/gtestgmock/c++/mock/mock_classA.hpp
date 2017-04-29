#ifndef _MOCK_CLASSA_H_
#define _MOCK_CLASSA_H_
// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::_;
using ::testing::Invoke;

#include "classA.hpp"

// ----------------------------------------------
class MockClassA : public ClassAInterface
{
	public:
		MOCK_METHOD1(doWork, void(int));
		MOCK_METHOD0(getProgress, int(void));

		MockClassA() {}
		virtual ~MockClassA() {}
};


// ----------------------------------------------
#endif // _MOCK_CLASSA_H_

