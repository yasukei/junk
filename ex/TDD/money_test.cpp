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
	EXPECT_EQ(Dollar(5), Dollar(5));
}

TEST(testEquality, 2)
{
	EXPECT_NE(Dollar(5), Dollar(6));
}

