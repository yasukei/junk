#include <typeinfo>
#include <cstdio>
#include <iostream>

#include "money.hpp"

// ------------------------------------------------------------------
// Expression
// ------------------------------------------------------------------
Expression::~Expression()
{
}

// ------------------------------------------------------------------
// Money
// ------------------------------------------------------------------
Money* Money::dollar(int amount)
{
	return new Money(amount, "USD"); // FIXME: use smart pointer
}

Money* Money::franc(int amount)
{
	return new Money(amount, "CHF"); // FIXME: use smart pointer
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

Expression* Money::plus(const Money& rhs) const
{
	return new Money(amount_ + rhs.amount_, currency_);
}

string Money::currency() const
{
	return currency_;
}

Money* Money::times(int multiplier) const
{
	return new Money(amount_ * multiplier, currency_); // FIXME: use smart pointer
}

// ------------------------------------------------------------------
// Bank
// ------------------------------------------------------------------
Money* Bank::reduce(Expression* source, string to)
{
	(void)source;
	(void)to;
	return Money::dollar(10);
}

