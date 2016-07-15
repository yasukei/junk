#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Dollar five(5);

	five.times(2);
	EXPECT_EQ(10, five.amount);
}

