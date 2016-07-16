#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Dollar five(5);

	EXPECT_EQ(Dollar(10), *five.times(2));
}

TEST(testMultiplication, 2)
{
	Dollar five(5);

	EXPECT_EQ(Dollar(15), *five.times(3));
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

