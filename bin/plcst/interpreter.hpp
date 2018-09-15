#pragma once

#include <string>
#include <set>
#include "iec.hpp"

namespace client
{
	struct VariableDecl
	{
		std::string var_name;
		std::string type_name;

		VariableDecl() :
			VariableDecl("", "")
		{
		}
		VariableDecl(const std::string name, const std::string type) :
			var_name(name),
			type_name(type)
		{
		}

		bool operator<(const VariableDecl& rhs) const
		{
			return var_name.compare(rhs.var_name) < 0 || type_name.compare(rhs.type_name) < 0;
		}
		bool operator==(const VariableDecl& rhs) const
		{
			return var_name.compare(rhs.var_name) == 0 && type_name.compare(rhs.type_name) == 0;
		}
	};
	struct VariablesDecl
	{
		std::set<VariableDecl> vars_decl;
	};
	struct Program
	{
		std::string name;
	};

	bool IdParser_parse(std::string input_str, std::string& parsed_output);
	bool VariableDeclParser_parse(std::string input_str, client::VariableDecl& parsed_output);
	bool VariablesDeclParser_parse(std::string input_str, client::VariablesDecl& parsed_output);
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
