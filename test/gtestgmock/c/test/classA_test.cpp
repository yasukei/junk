// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;

#include "mock_util.hpp"
#include "classA.h"

// ----------------------------------------------
class ClassA_create_test : public ::testing::Test
{
	protected:
		MockUtil mock;

		virtual void SetUp()
		{
			MockUtil::setInstance(&mock);
		}
		virtual void TearDown()
		{
			MockUtil::setInstance(nullptr);
		}
};

// ----------------------------------------------
TEST_F(ClassA_create_test, success1)
{
	ClassA* classA;

	// precondition
	EXPECT_CALL(mock, UtilMalloc(_));
	EXPECT_CALL(mock, UtilFree(_));

	// target
	classA = ClassA_create();

	// postcondition
	EXPECT_THAT(classA, NotNull());
	ClassA_destroy(classA);
}

// ----------------------------------------------
TEST_F(ClassA_create_test, error1)
{
	ClassA* classA;

	// precondition
	EXPECT_CALL(mock, UtilMalloc(_))
		.WillOnce(Return((void*)NULL));
	//EXPECT_CALL(mock, UtilFree(_));

	// target
	classA = ClassA_create();

	// postcondition
	EXPECT_THAT(classA, IsNull());
	//ClassA_destroy(classA);
}

