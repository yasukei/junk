#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Dollar five(5);

	EXPECT_EQ(Dollar(10), *(five.times(2)));
	EXPECT_EQ(Dollar(15), *(five.times(3)));
}

TEST(testEquality, 1)
{
	EXPECT_EQ(Dollar(5), Dollar(5));
	EXPECT_NE(Dollar(5), Dollar(6));
}

TEST(testFrancMultiplication, 1)
{
	Franc five(5);

	EXPECT_EQ(Franc(10), *(five.times(2)));
	EXPECT_EQ(Franc(15), *(five.times(3)));
}

TEST(testFrancEquality, 1)
{
	EXPECT_EQ(Franc(5), Franc(5));
	EXPECT_NE(Franc(5), Franc(6));
}

