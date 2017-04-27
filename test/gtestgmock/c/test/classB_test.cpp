// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Return;

#include "mock_util.hpp"
#include "mock_classA.hpp"
#include "classB.h"

// ----------------------------------------------
class ClassB_create_test : public ::testing::Test
{
	protected:
		MockUtil mockUtil;
		MockClassA mockClassA;

		virtual void SetUp()
		{
			MockUtil::setInstance(&mockUtil);
		}
		virtual void TearDown()
		{
			MockUtil::setInstance(nullptr);
		}
};

// ----------------------------------------------
TEST_F(ClassB_create_test, success1)
{
	ClassB* classB;

	// precondition
	EXPECT_CALL(mockUtil, UtilMalloc(_));
	EXPECT_CALL(mockUtil, UtilFree(_));

	// target
	classB = ClassB_create((ClassA*)&mockClassA);

	// postcondition
	EXPECT_THAT(classB, NotNull());
	ClassB_destroy(classB);
}

// ----------------------------------------------
TEST_F(ClassB_create_test, error1)
{
	ClassB* classB;

	// precondition
	EXPECT_CALL(mockUtil, UtilMalloc(_))
		.WillOnce(Return((void*)NULL));

	// target
	classB = ClassB_create((ClassA*)&mockClassA);

	// postcondition
	EXPECT_THAT(classB, IsNull());
}

