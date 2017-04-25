// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::NotNull;

#include "classA.h"

// ----------------------------------------------
TEST(ClassA_create_test, success1)
{
	ClassA* classA;

	// precondition

	// target
	classA = ClassA_create();

	// postcondition
	EXPECT_THAT(classA, NotNull());
	ClassA_destroy(classA);
}

