#pragma once

#include <string>
#include "iec.hpp"

namespace client
{
	struct VariableDecl
	{
		std::string var_name;
		std::string type_name;
	};
	struct Program
	{
		std::string name;
	};

	bool IdParser_parse(std::string input_str, std::string& parsed_output);
	bool VariableDeclParser_parse(std::string input_str, client::VariableDecl& parsed_output);
}

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
