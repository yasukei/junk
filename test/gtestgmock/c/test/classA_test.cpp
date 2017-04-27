// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::Eq;

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

	// target
	classA = ClassA_create();

	// postcondition
	EXPECT_THAT(classA, IsNull());
}

// ----------------------------------------------
typedef ClassA_create_test ClassA_doWork_test;

// ----------------------------------------------
TEST_F(ClassA_doWork_test, success1)
{
	ClassA* classA;
	int preProgress;
	int postProgress;

	// precondition
	EXPECT_CALL(mock, UtilMalloc(_));
	EXPECT_CALL(mock, UtilFree(_));
	classA = ClassA_create();
	ASSERT_THAT(classA, NotNull());
	preProgress = ClassA_getProgress(classA);

	// target
	ClassA_doWork(classA, 0);

	// postcondition
	postProgress = ClassA_getProgress(classA);
	EXPECT_THAT(postProgress, Eq(preProgress));
	ClassA_destroy(classA);
}

