#include <iostream>
#include "interpreter.hpp"

int main(
	int ac,
	const char* av[]
	)
{
	Interpreter itp;
	std::string str("PROGRAM a1a END_PROGRAM");
	//std::string str("PROGRAM a1a VAR x : INT ; VAR_END END_PROGRAM");

	itp.interpret(str);

	return EXIT_SUCCESS;
}

