#ifndef _MONEY_HPP_
#define _MONEY_HPP_

class Dollar
{
	public:
		int amount;

		Dollar(int amount) :
			amount(10)
		{
			(void)amount;
		}

		void times(int multiplier)
		{
			(void)multiplier;
		}
};

#endif // _MONEY_HPP_
