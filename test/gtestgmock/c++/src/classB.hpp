#ifndef _CLASSB_HPP_
#define _CLASSB_HPP_
// ----------------------------------------------

#include "classA.hpp"

class ClassB
{
	public:
		ClassB(ClassAInterface& classA);
		~ClassB(void);

		void doWork(int money);
		int getProgress(void);

	protected:
		int progress_;
		int money_;
		ClassAInterface& classA_;
};

// ----------------------------------------------
#endif // _CLASSB_HPP_
