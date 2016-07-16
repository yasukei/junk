#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	Money* five1 = Money::dollar(5);
	EXPECT_EQ(*Money::dollar(10), *(five1->times(2)));
	EXPECT_EQ(*Money::dollar(15), *(five1->times(3)));

	Money* five2 = Money::franc(5);
	EXPECT_EQ(*Money::franc(10), *(five2->times(2)));
	EXPECT_EQ(*Money::franc(15), *(five2->times(3)));
}

TEST(testEquality, 1)
{
	EXPECT_EQ(*Money::dollar(5), *Money::dollar(5));
	EXPECT_NE(*Money::dollar(5), *Money::dollar(6));

	EXPECT_EQ(*Money::franc(5), *Money::franc(5));
	EXPECT_NE(*Money::franc(5), *Money::franc(6));

	EXPECT_NE(*Money::dollar(5), *Money::franc(5));
}

TEST(testCurrency, 1)
{
	EXPECT_EQ("USD", Money::dollar(1)->currency());
	EXPECT_EQ("CHF", Money::franc(1)->currency());
}

TEST(testDifferentClassEquality, 1)
{
	EXPECT_EQ(Money(10, "CHF"), *Money::franc(10));
}

