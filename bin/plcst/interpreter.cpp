#include <vector>
#include "parser.hpp"

#include "interpreter.hpp"

void Interpreter::interpret(
	std::string str
	)
{
	typedef std::string::const_iterator iterator_type;
	typedef client::program_parser<iterator_type> program_parser;

	program_parser g; // our grammar

	std::string::const_iterator start = str.begin();
	std::string::const_iterator end = str.end();

	bool r = phrase_parse(start, end, g, boost::spirit::ascii::space, program_);
	if(r && start == end)
	{
		//std::cout << "succeeded\n";
		std::cout << boost::fusion::as_vector(program_) << std::endl;
	}
	else
	{
		std::cout << "failed\n";
	}
}

const client::program& Interpreter::getProgram()
{
	return program_;
}

