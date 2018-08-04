#pragma once

#include <string>
#include "parser.hpp"

//-------------------------------------------------------------------
// Interpreter
//-------------------------------------------------------------------
class Interpreter
{
public:
	Interpreter() {};
	~Interpreter() {};

	void interpret(std::string str);
	const client::program& getProgram();
private:
	client::program program_;
};
