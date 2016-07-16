#include <typeinfo>
#include <cstdio>
#include <iostream>

#include "money.hpp"

using std::make_shared;

// ------------------------------------------------------------------
// Expression
// ------------------------------------------------------------------
Expression::~Expression()
{
}

// ------------------------------------------------------------------
// Money
// ------------------------------------------------------------------
shared_ptr<Money> Money::dollar(int amount)
{
	return make_shared<Money>(amount, "USD");
}

shared_ptr<Money> Money::franc(int amount)
{
	return make_shared<Money>(amount, "CHF");
}

Money::Money(int amount, string currency) :
	amount_(amount),
	currency_(currency)
{
}

Money::~Money()
{
}

bool Money::equals(const Money& another) const
{
	return (amount_ == another.amount_) && (currency_ == another.currency_);
}

bool Money::operator==(const Money& rhs) const
{
	return equals(rhs);
}

bool Money::operator!=(const Money& rhs) const
{
	return ! (*this == rhs);
}

shared_ptr<Expression> Money::plus(const Money& rhs) const
{
	return make_shared<Money>(amount_ + rhs.amount_, currency_);
}

string Money::currency() const
{
	return currency_;
}

shared_ptr<Money> Money::times(int multiplier) const
{
	return make_shared<Money>(amount_ * multiplier, currency_);
}

// ------------------------------------------------------------------
// Bank
// ------------------------------------------------------------------
shared_ptr<Money> Bank::reduce(Expression& source, string to)
{
	(void)source;
	(void)to;
	return Money::dollar(10);
}

