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

		Dollar* times(int multiplier) const
		{
			return new Dollar(amount_ * multiplier);
		}

		bool equals(const Dollar& another) const
		{
			return amount_ == another.amount_;
		}

		bool operator==(const Dollar& rhs) const
		{
			return equals(rhs);
		}

		bool operator!=(const Dollar& rhs) const
		{
			return ! (*this == rhs);
		}
};

#endif // _MONEY_HPP_
