#ifndef _MONEY_HPP_
#define _MONEY_HPP_

#include <typeinfo>
#include <cstdio>
#include <iostream>

class Money
{
	public:
		Money(int amount)
			: amount_(amount)
		{
		}
		virtual ~Money() = default;

		virtual bool equals(const Money& another) const
		{
			return (amount_ == another.amount_) && (typeid(*this) == typeid(another));
		}

		virtual bool operator==(const Money& rhs) const
		{
			return equals(rhs);
		}

		virtual bool operator!=(const Money& rhs) const
		{
			return ! (*this == rhs);
		}

		virtual Money* times(int multiplier) const
		{
			return new Money(amount_ * multiplier); // FIXME: use smart pointer
		}

	protected:
		int amount_;
};

class Dollar : public Money
{
	public:
		Dollar(int amount)
			: Money(amount)
		{
		}

		virtual Dollar* times(int multiplier) const
		{
			return new Dollar(amount_ * multiplier); // FIXME: use smart pointer
		}

};

class Franc : public Money
{
	public:
		Franc(int amount)
			: Money(amount)
		{
		}

		virtual Franc* times(int multiplier) const
		{
			return new Franc(amount_ * multiplier); // FIXME: use smart pointer
		}

};

#endif // _MONEY_HPP_
