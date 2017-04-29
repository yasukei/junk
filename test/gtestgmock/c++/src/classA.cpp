#include "classA.hpp"

// ----------------------------------------------
ClassA::ClassA(void) :
	progress_(0)
{
}

// ----------------------------------------------
ClassA::~ClassA(void)
{
}

// ----------------------------------------------
void ClassA::doWork(int money)
{
	if(money < 0)
	{
		return;
	}
	this->progress_ = money;
}

// ----------------------------------------------
int ClassA::getProgress(void)
{
	return this->progress_;
}

