#include <stdio.h>

//-------------------------------------------------------------------
class MyInterface
{
	public:
		virtual ~MyInterface() = 0;

		virtual int get();
		virtual void set(int value);
};

MyInterface::~MyInterface()
{
}

int MyInterface::get()
{
}

void MyInterface::set(int value)
{
}

//-------------------------------------------------------------------
class MyImple1 : public MyInterface
{
	public:
		MyImple1() : value_(0) {}
		~MyImple1() {}

		virtual int get()			{ return value_; }
		virtual void set(int value)	{ value_ = value; }

	private:
		int value_;
};

//-------------------------------------------------------------------
class MyImple2 : public MyInterface
{
	public:
		MyImple2() : value_(0) {}
		~MyImple2() {}

		virtual int get()			{ return value_; }
		virtual void set(int value)	{ value_ = 2 * value; }

	private:
		int value_;
};

//-------------------------------------------------------------------
int main(void)
{
	MyImple1 imple1;
	MyImple2 imple2;
	MyInterface* myInterface = &imple1;

	myInterface->set(1);
	printf("hello %d\n", myInterface->get());
	return 0;
}
