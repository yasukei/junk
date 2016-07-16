#ifndef _MONEY_HPP_
#define _MONEY_HPP_

class Money;
class Dollar;
class Frank;

class Money
{
	public:
		// factory method
		static Money* dollar(int amount);
		static Money* franc(int amount);

		// methods for inheritance
		Money(int amount);
		virtual ~Money() = default;

		virtual bool equals(const Money& another) const;
		virtual bool operator==(const Money& rhs) const;
		virtual bool operator!=(const Money& rhs) const;
		virtual Money* times(int multiplier) const = 0;

	protected:
		int amount_;
};

class Dollar : public Money
{
	public:
		Dollar(int amount);

		virtual Money* times(int multiplier) const;

};

class Franc : public Money
{
	public:
		Franc(int amount);

		virtual Money* times(int multiplier) const;

};

#endif // _MONEY_HPP_
