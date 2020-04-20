#include <cstdio>
#include <string>

#include <thread>

class SomeClass
{
public:
	SomeClass(std::string threadName, int loopNum) :
		_threadName(threadName),
		_loopNum(loopNum)
	{
	}

	static void run(void* arg)
	{
		SomeClass* self = (SomeClass*)arg;

		for (int i = 0; i < self->_loopNum; i++)
		{
			printf("threadName: [%s], i: [%d]\n", self->_threadName.c_str(), i);
		}
	}

private:
	std::string _threadName;
	int _loopNum;
};

int main()
{
	SomeClass instance1("thread1", 5);
	SomeClass instance2("thread2", 10);

	std::thread thread1(SomeClass::run, &instance1);
	std::thread thread2(SomeClass::run, &instance2);

	printf("thread1.joinable(): [%d]\n", thread1.joinable());
	printf("thread2.joinable(): [%d]\n", thread2.joinable());

	printf("before join\n");
	thread1.join();
	thread2.join();
	printf("after join\n");
	return 0;
}
