#pragma once

#include <string>
#include "iec.hpp"

//-------------------------------------------------------------------
// Interpreter
//-------------------------------------------------------------------
class Interpreter
{
public:
	Interpreter();
	~Interpreter();

	void interpret(std::string str);
	iec::Program* getProgram();

private:
	iec::Program* program_;
};
