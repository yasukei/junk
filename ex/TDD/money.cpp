#include <typeinfo>
#include <cstdio>
#include <iostream>

#include "money.hpp"

// ------------------------------------------------------------------
// Money
// ------------------------------------------------------------------
Money* Money::dollar(int amount)
{
	return new Dollar(amount, "USD"); // FIXME: use smart pointer
}

Money* Money::franc(int amount)
{
	return new Franc(amount, "CHF"); // FIXME: use smart pointer
}

Money::Money(int amount, string currency) :
	amount_(amount),
	currency_(currency)
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

string Money::currency() const
{
	return currency_;
}

Money* Money::times(int multiplier) const
{
	return new Money(amount_ * multiplier, currency_); // FIXME: use smart pointer
}

// ------------------------------------------------------------------
// Dollar
// ------------------------------------------------------------------
Dollar::Dollar(int amount, string currency) :
	Money(amount, currency)
{
}

Money* Dollar::times(int multiplier) const
{
	return new Money(amount_ * multiplier, currency_); // FIXME: use smart pointer
}

// ------------------------------------------------------------------
// Franc
// ------------------------------------------------------------------
Franc::Franc(int amount, string currency) :
	Money(amount, currency)
{
}

Money* Franc::times(int multiplier) const
{
	return new Money(amount_ * multiplier, currency_); // FIXME: use smart pointer
}

