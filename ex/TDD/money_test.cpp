#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Dollar five(5);
	Dollar* product = five.times(2);

	EXPECT_EQ(10, product->amount_);
}

TEST(testMultiplication, 2)
{
	Dollar five(5);
	Dollar* product = five.times(3);

	EXPECT_EQ(15, product->amount_);
}

TEST(testEquality, 1)
{
	Dollar five1(5);
	Dollar five2(5);
	EXPECT_EQ(five1, five2);
}

TEST(testEquality, 2)
{
	Dollar five1(5);
	Dollar five2(6);
	EXPECT_NE(five1, five2);
}

