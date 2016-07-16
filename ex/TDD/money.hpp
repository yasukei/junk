#ifndef _MONEY_HPP_
#define _MONEY_HPP_

#include <string>

using std::string;

class Expression;
class Money;
class Bank;

class Expression
{
	public:
		virtual ~Expression() = 0;
};

class Money : public Expression
{
	public:
		// factory method
		static Money* dollar(int amount);
		static Money* franc(int amount);

		Money(int amount, string currency);
		~Money();

		bool equals(const Money& another) const;
		bool operator==(const Money& rhs) const;
		bool operator!=(const Money& rhs) const;
		Expression* plus(const Money& another) const;
		string currency() const;
		Money* times(int multiplier) const;

	protected:
		int amount_;
		string currency_;

};

class Bank
{
	public:
		Money* reduce(Expression* source, string to);
};

#endif // _MONEY_HPP_
