#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>

#include "iec.hpp"
#include "parser.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

void print_double(double d)
{
	std::cout << "number: [" << d << "]\n";
}

template <typename Iterator>
bool parse_numbers(Iterator first, Iterator last, double& n)
{
	using qi::double_;
	using qi::_1;
	using qi::phrase_parse;
	using ascii::space;
	using phoenix::ref;
	//double d;

	bool r = phrase_parse(
				first,
				last,
				//double_[print_double] >> *(',' >> double_[print_double]),
				double_[print_double] % ',',
				//double_[ref(n) = _1] >> *(',' >> double_[ref(n) += _1]),
				space
				);

	if(first != last)
	{
		return false;
	}

	return r;
}

template <typename Iterator>
bool adder(Iterator first, Iterator last, double& n)
{
	using qi::double_;
	using qi::_1;
	using qi::phrase_parse;
	using ascii::space;
	using phoenix::ref;

    //bool r = qi::phrase_parse(first, last,
    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
            double_[ref(n) = _1] >> *(',' >> double_[ref(n) += _1])
        )
        ,
        //  End grammar

        space);

    if (first != last) // fail if we did not get a full match
        return false;
    return r;
}


