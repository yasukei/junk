#include <typeinfo>
#include <cstdio>
#include <iostream>

#include "money.hpp"

// ------------------------------------------------------------------
// Money
// ------------------------------------------------------------------
Money* Money::dollar(int amount)
{
	return new Dollar(amount);
}

Money* Money::franc(int amount)
{
	return new Franc(amount);
}

Money::Money(int amount)
	: amount_(amount)
{
}

bool Money::equals(const Money& another) const
{
	return (amount_ == another.amount_) && (typeid(*this) == typeid(another));
}

bool Money::operator==(const Money& rhs) const
{
	return equals(rhs);
}

bool Money::operator!=(const Money& rhs) const
{
	return ! (*this == rhs);
}

// ------------------------------------------------------------------
// Dollar
// ------------------------------------------------------------------
Dollar::Dollar(int amount)
	: Money(amount)
{
}

Money* Dollar::times(int multiplier) const
{
	return new Dollar(amount_ * multiplier); // FIXME: use smart pointer
}

// ------------------------------------------------------------------
// Franc
// ------------------------------------------------------------------
Franc::Franc(int amount)
	: Money(amount)
{
}

Money* Franc::times(int multiplier) const
{
	return new Franc(amount_ * multiplier); // FIXME: use smart pointer
}
