#pragma once

#include <string>
#include "parser.hpp"

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
