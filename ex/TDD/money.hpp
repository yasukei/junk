#ifndef _MONEY_HPP_
#define _MONEY_HPP_

#include <string>

using std::string;

class Money;
class Dollar;
class Frank;

class Money
{
	public:
		// factory method
		static Money* dollar(int amount);
		static Money* franc(int amount);

		// methods for subclasses
		Money(int amount, string currency);
		virtual ~Money() = default;

		bool equals(const Money& another) const;
		bool operator==(const Money& rhs) const;
		bool operator!=(const Money& rhs) const;
		string currency() const;
		virtual Money* times(int multiplier) const = 0;

	protected:
		int amount_;
		string currency_;

};

class Dollar : public Money
{
	public:
		Dollar(int amount, string currency);

		virtual Money* times(int multiplier) const;

};

class Franc : public Money
{
	public:
		Franc(int amount, string currency);

		virtual Money* times(int multiplier) const;

};

#endif // _MONEY_HPP_
