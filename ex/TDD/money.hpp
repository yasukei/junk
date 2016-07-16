#ifndef _MONEY_HPP_
#define _MONEY_HPP_

class Dollar
{
	public:
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

	private:
		int amount_;
};

class Franc
{
	public:
		Franc(int amount) :
			amount_(amount)
		{
		}

		Franc* times(int multiplier) const
		{
			return new Franc(amount_ * multiplier);
		}

		bool equals(const Franc& another) const
		{
			return amount_ == another.amount_;
		}

		bool operator==(const Franc& rhs) const
		{
			return equals(rhs);
		}

		bool operator!=(const Franc& rhs) const
		{
			return ! (*this == rhs);
		}

	private:
		int amount_;
};

#endif // _MONEY_HPP_
