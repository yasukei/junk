// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Gt;
using ::testing::Le;
using ::testing::Lt;
using ::testing::Ne;

#include "classA.hpp"

// ----------------------------------------------
class ClassA_doWork_test : public ::testing::Test
{
	protected:
		virtual void SetUp()
		{
		}
		virtual void TearDown()
		{
		}
};

// ----------------------------------------------
TEST_F(ClassA_doWork_test, case0)
{
	ClassA classA;
	int preProgress;
	int postProgress;

	// precondition
	preProgress = classA.getProgress();

	// target
	classA.doWork(-1);

	// postcondition
	postProgress = classA.getProgress();
	EXPECT_THAT(postProgress, Eq(preProgress));
}

// ----------------------------------------------
TEST_F(ClassA_doWork_test, case1)
{
	ClassA classA;
	int preProgress;
	int postProgress;

	// precondition
	preProgress = classA.getProgress();

	// target
	classA.doWork(0);

	// postcondition
	postProgress = classA.getProgress();
	EXPECT_THAT(postProgress, Eq(preProgress));
}

// ----------------------------------------------
TEST_F(ClassA_doWork_test, case2)
{
	ClassA classA;
	int preProgress;
	int postProgress;

	// precondition
	preProgress = classA.getProgress();

	// target
	classA.doWork(1);

	// postcondition
	postProgress = classA.getProgress();
	EXPECT_THAT(postProgress, Gt(preProgress));
}

// ----------------------------------------------
TEST_F(ClassA_doWork_test, case3)
{
	ClassA classA;
	int preProgress;
	int postProgress;

	// precondition
	preProgress = classA.getProgress();

	// target
	classA.doWork(2);

	// postcondition
	postProgress = classA.getProgress();
	EXPECT_THAT(postProgress, Gt(preProgress));
}

