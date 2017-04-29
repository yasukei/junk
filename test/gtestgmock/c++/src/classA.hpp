#ifndef _CLASSA_HPP_
#define _CLASSA_HPP_
// ----------------------------------------------

class ClassAInterface
{
	public:
		virtual ~ClassAInterface() {};

		virtual void doWork(int money) = 0;
		virtual int getProgress(void) = 0;
};

class ClassA : public ClassAInterface
{
	public:
		ClassA(void);
		virtual ~ClassA(void);

		virtual void doWork(int money);
		virtual int getProgress(void);

	protected:
		int progress_;
};

// ----------------------------------------------
#endif // _CLASSA_HPP_
