#include "money.hpp"
#include <gtest/gtest.h>

TEST(testMultiplication, 1)
{
	shared_ptr<Money> five = Money::dollar(5);
	EXPECT_EQ(*Money::dollar(10), *(five->times(2)));
	EXPECT_EQ(*Money::dollar(15), *(five->times(3)));
}

TEST(testSimpleAddition, 1)
{
	shared_ptr<Money> five = Money::dollar(5);
	shared_ptr<Expression> sum = five->plus(*five);
	shared_ptr<Bank> bank = std::make_shared<Bank>();
	shared_ptr<Money> reduced = bank->reduce(*sum, "USD");
	EXPECT_EQ(*Money::dollar(10), *reduced);
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

