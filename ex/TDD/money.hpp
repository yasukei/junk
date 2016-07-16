#ifndef _MONEY_HPP_
#define _MONEY_HPP_

class Money
{
	public:
		Money(int amount)
			: amount_(amount)
		{
		}

		bool equals(const Money& another) const
		{
			return amount_ == another.amount_;
		}

		bool operator==(const Money& rhs) const
		{
			return equals(rhs);
		}

		bool operator!=(const Money& rhs) const
		{
			return ! (*this == rhs);
		}

		Money* times(int multiplier) const
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
};

class Franc : public Money
{
	public:
		Franc(int amount)
			: Money(amount)
		{
		}
};

#endif // _MONEY_HPP_
