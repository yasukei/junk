#ifndef _MONEY_HPP_
#define _MONEY_HPP_

class Dollar
{
	public:
		int amount_;

		Dollar(int amount) :
			amount_(amount)
		{
		}

		Dollar* times(int multiplier)
		{
			return new Dollar(amount_ * multiplier);
		}
		bool equals(Dollar& another)
		{
			return amount_ == another.amount_;
		}
};

#endif // _MONEY_HPP_
