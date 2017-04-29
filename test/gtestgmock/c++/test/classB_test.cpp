// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Gt;
using ::testing::Le;
using ::testing::Lt;
using ::testing::Ne;

#include "mock_classA.hpp"
#include "classB.hpp"

// ----------------------------------------------
class ClassB_doWork_test : public ::testing::Test
{
	protected:
		int preProgress;
		int postProgress;

		virtual void SetUp()
		{
		}
		virtual void TearDown()
		{
		}
};

// ----------------------------------------------
TEST_F(ClassB_doWork_test, case0)
{
	MockClassA mockClassA;
	ClassB classB(mockClassA);
	EXPECT_CALL(mockClassA, getProgress())
		.Times(AtLeast(1));
	EXPECT_CALL(mockClassA, doWork(_))
		.Times(AtLeast(0));

	// precondition
	preProgress = classB.getProgress();

	// target
	classB.doWork(-1);

	// postcondition
	postProgress = classB.getProgress();
	EXPECT_THAT(postProgress, Eq(preProgress));
}

// ----------------------------------------------
TEST_F(ClassB_doWork_test, case1)
{
	MockClassA mockClassA;
	ClassB classB(mockClassA);
	EXPECT_CALL(mockClassA, getProgress())
		.Times(AtLeast(1));
	EXPECT_CALL(mockClassA, doWork(_))
		.Times(AtLeast(0));

	// precondition
	preProgress = classB.getProgress();

	// target
	classB.doWork(0);

	// postcondition
	postProgress = classB.getProgress();
	EXPECT_THAT(postProgress, Eq(preProgress));
}

// ----------------------------------------------
TEST_F(ClassB_doWork_test, case2)
{
	MockClassA mockClassA;
	ClassB classB(mockClassA);
	EXPECT_CALL(mockClassA, getProgress())
		.Times(AtLeast(1));
	EXPECT_CALL(mockClassA, doWork(_))
		.Times(AtLeast(1));

	// precondition
	preProgress = classB.getProgress();

	// target
	classB.doWork(1);

	// postcondition
	postProgress = classB.getProgress();
	EXPECT_THAT(postProgress, Eq(preProgress));
}

// ----------------------------------------------
TEST_F(ClassB_doWork_test, case3)
{
	MockClassA mockClassA;
	ClassB classB(mockClassA);
	EXPECT_CALL(mockClassA, getProgress())
		.Times(AtLeast(1));
	EXPECT_CALL(mockClassA, doWork(_))
		.Times(AtLeast(1));

	// precondition
	preProgress = classB.getProgress();

	// target
	classB.doWork(2);

	// postcondition
	postProgress = classB.getProgress();
	EXPECT_THAT(postProgress, Ge(preProgress));
}

