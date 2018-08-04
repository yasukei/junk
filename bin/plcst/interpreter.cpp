#include <iostream>
#include <string>
#include <vector>
#include "interpreter.hpp"
#include "parser.hpp"
#include "iec.hpp"


#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

static void print_double(double d)
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

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	client::program,
	(std::string, name)
)

namespace client
{
	template <typename Iterator>
	struct program_parser : qi::grammar<Iterator, program(), ascii::space_type>
	{
		program_parser() : program_parser::base_type(start)
		{
			using qi::lit;
			using qi::int_;
			using qi::double_;
			using qi::lexeme;
			using qi::on_error;
			using qi::fail;
			using qi::retry;
			using qi::accept;
			using qi::rethrow;
			using ascii::char_;
			using ascii::alpha;
			using ascii::alnum;
			using ascii::space;
			using namespace qi::labels;

			using phoenix::construct;
			using phoenix::val;

			start %=
				lit("PROGRAM")
				//>> *(char_)
				//>> lexeme["aaa"]						// success
				//>> lexeme[char_ >> char_ >> char_]	// success
				> lexeme[(alpha) > *(alnum)]	// success
				> lexeme["END_PROGRAM"]
				;

			//start.name("start");

			on_error<fail>
				(
				 start,
				 std::cout
				 	<< val("Error! Expecting ")
					<< _4
					<< val(" here: \"")
					<< construct<std::string>(_3, _2)
					<< val("\"")
					<< std::endl
				);
		}

		qi::rule<Iterator, program(), ascii::space_type> start;
	};
}

//-------------------------------------------------------------------
// Interpreter
//-------------------------------------------------------------------
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

