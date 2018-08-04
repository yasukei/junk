#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "interpreter.hpp"
#include "parser.hpp"

#define cPROGRAM		("PROGRAM")
#define cEND_PROGRAM	("END_PROGRAM")

class ConcatString
{
public:
	ConcatString(std::string delimiter) :
		str_(""),
		delimiter_(delimiter)
	{
	}

	void operator+=(const std::string str)
	{
		str_ += delimiter_ + str;
	}

	std::string getString()
	{
		return str_;
	}

private:
	std::string str_;
	std::string delimiter_;
};

TEST(aaa, bbb)
{
	Interpreter itp;
	ConcatString str(" ");
	const std::string program_name("myprogram1");

	str += cPROGRAM;
	str += program_name;
	str += cEND_PROGRAM;

	//std::string str("PROGRAM a1a END_PROGRAM");
	//std::string str("PROGRAM a1a VAR x : INT ; VAR_END END_PROGRAM");

	itp.interpret(str.getString());
	const client::program p(itp.getProgram());
	EXPECT_EQ(program_name, p.name);
}

