#include <iostream>
#include <string>
#include <vector>

#include "parser.hpp"

int main(int ac, const char* av[])
{
	std::cout << "hello" << std::endl;

	std::string str("3.8,\n3");
	double d;

	if(parse_numbers(str.begin(), str.end(), d))
	{
		std::cout << "succeeded\n";
	}
	else
	{
		std::cout << "failed\n";
	}

	return EXIT_SUCCESS;
}

