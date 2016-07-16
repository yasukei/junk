#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Money* five = Money::dollar(5);
	EXPECT_EQ(*Money::dollar(10), *(five->times(2)));
	EXPECT_EQ(*Money::dollar(15), *(five->times(3)));
}

TEST(testEquality, 1)
{
	EXPECT_EQ(*Money::dollar(5), *Money::dollar(5));
	EXPECT_NE(*Money::dollar(5), *Money::dollar(6));

	EXPECT_NE(*Money::dollar(5), *Money::franc(5));
}

TEST(testCurrency, 1)
{
	EXPECT_EQ("USD", Money::dollar(1)->currency());
	EXPECT_EQ("CHF", Money::franc(1)->currency());
}

