#include "Commands.hpp"

// ------------------------------------------------------------------
// TestCommand1
// ------------------------------------------------------------------
class TestCommand1 : public Command
{
	public:
		TestCommand1(void);
		~TestCommand1(void);

		bool execute(const string& arg, ostream& os);
};

// ------------------------------------------------------------------
TestCommand1::TestCommand1(void)
{
}

// ------------------------------------------------------------------
TestCommand1::~TestCommand1(void)
{
}

// ------------------------------------------------------------------
bool TestCommand1::execute(const string& arg, ostream& os)
{
	os << arg << endl;
	return true;
}

extern "C"
{

// ------------------------------------------------------------------
Command* TestCommand1_create(void)
{
	return new TestCommand1();
}

// ------------------------------------------------------------------
void TestCommand1_destroy(Command* command)
{
	delete(command);
}

} /* extern "C" */

