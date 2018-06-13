#include <iostream>
#include <string>
#include <vector>

#include "parser.hpp"

int main(int ac, const char* av[])
{
	//std::cout << "hello" << std::endl;

	//std::string str("3.8,\n3");
	//double d;

	typedef std::string::const_iterator iterator_type;
	typedef client::program_parser<iterator_type> program_parser;

	program_parser g; // our grammar
	std::string str("PROGRAM a1a END_PROGRAM");

	client::program prg;
	std::string::const_iterator start = str.begin();
	std::string::const_iterator end = str.end();

	bool r = phrase_parse(start, end, g, boost::spirit::ascii::space, prg);
	if(r && start == end)
	{
		std::cout << "succeeded\n";
		std::cout << boost::fusion::as_vector(prg) << std::endl;
	}
	else
	{
		std::cout << "failed\n";
	}

	return EXIT_SUCCESS;
}

