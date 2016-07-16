#ifndef _MONEY_HPP_
#define _MONEY_HPP_

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

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
		static shared_ptr<Money> dollar(int amount);
		static shared_ptr<Money> franc(int amount);

		Money(int amount, string currency);
		~Money();

		bool equals(const Money& another) const;
		bool operator==(const Money& rhs) const;
		bool operator!=(const Money& rhs) const;
		shared_ptr<Expression> plus(const Money& another) const;
		string currency() const;
		shared_ptr<Money> times(int multiplier) const;

	protected:
		int amount_;
		string currency_;

};

class Bank
{
	public:
		shared_ptr<Money> reduce(Expression& source, string to);
};

#endif // _MONEY_HPP_
