#include "classB.hpp"

// ----------------------------------------------
ClassB::ClassB(ClassAInterface& classA) :
	progress_(0),
	classA_(classA)
{
}

// ----------------------------------------------
ClassB::~ClassB(void)
{
}

// ----------------------------------------------
void ClassB::doWork(int money)
{
	if(money < 0)
	{
		return;
	}
	this->classA_.doWork(money / 2);
	this->progress_ = money / 4;
	this->money_ = money / 4;
}

// ----------------------------------------------
int ClassB::getProgress(void)
{
	return this->progress_ + this->classA_.getProgress();
}


