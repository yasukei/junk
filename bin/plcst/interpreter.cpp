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
	client::VariableDecl,
	(std::string, var_name),
	(std::string, type_name)
)

BOOST_FUSION_ADAPT_STRUCT(
	client::VariablesDecl,
	(std::set<client::VariableDecl>, vars_decl)
)

BOOST_FUSION_ADAPT_STRUCT(
	client::Program,
	(std::string, name)
)

namespace client
{
	using qi::lexeme;
	using qi::on_error;
	using qi::fail;
	using ascii::alpha;
	using ascii::alnum;
	using phoenix::val;
	using phoenix::at_c;
	using phoenix::push_back;
	using phoenix::insert;
	using phoenix::construct;
	using namespace qi::labels;

	typedef std::string::const_iterator iterator_type;

#define id_grammar	id %= lexeme[+(alpha) > *(alnum)]
#define id_rule		qi::rule<Iterator, std::string(), ascii::space_type> id
#define variable_decl_grammar										\
			variable_decl =											\
							id			[at_c<0>(_val) = _1]		\
						>> ":"										\
						>> id			[at_c<1>(_val) = _1]		
#define variable_decl_rule	qi::rule<Iterator, VariableDecl(), ascii::space_type> variable_decl


	//-------------------------------------------------------------------
	// IdParser
	//-------------------------------------------------------------------
	template <typename Iterator>
	struct IdParser : qi::grammar<Iterator, std::string(), ascii::space_type>
	{
		IdParser() : IdParser::base_type(id)
		{
			id_grammar;
		}

		id_rule;
	};

	//-------------------------------------------------------------------
	bool IdParser_parse(std::string input_str, std::string& parsed_output)
	{
		client::IdParser<iterator_type> grammar;
		iterator_type start(input_str.begin());
		iterator_type end(input_str.end());

		bool result(phrase_parse(start, end, grammar, boost::spirit::ascii::space, parsed_output));
		return result && start == end;
	}

	//-------------------------------------------------------------------
	// VariableDeclParser
	//-------------------------------------------------------------------
	template <typename Iterator>
	struct VariableDeclParser : qi::grammar<Iterator, VariableDecl(), ascii::space_type>
	{
		VariableDeclParser() : VariableDeclParser::base_type(variable_decl)
		{
			id_grammar;
			variable_decl_grammar;
		}

		id_rule;
		variable_decl_rule;
	};

	//-------------------------------------------------------------------
	bool VariableDeclParser_parse(std::string input_str, client::VariableDecl& parsed_output)
	{
		client::VariableDeclParser<iterator_type> grammar;
		iterator_type start(input_str.begin());
		iterator_type end(input_str.end());

		bool result(phrase_parse(start, end, grammar, boost::spirit::ascii::space, parsed_output));
		return result && start == end;
	}

	//-------------------------------------------------------------------
	// VariablesDeclParser
	//-------------------------------------------------------------------
	template <typename Iterator>
	struct VariablesDeclParser : qi::grammar<Iterator, VariablesDecl(), ascii::space_type>
	{
		VariablesDeclParser() : VariablesDeclParser::base_type(variables_decl)
		{
			id_grammar;
			variable_decl_grammar;
			variables_decl =
								"VAR"
							>> variable_decl	[insert(at_c<0>(_val), _1)]
							//>> *variable_decl	[insert(at_c<0>(_val), _1)]
							>> "END_VAR"								
							;

			id.name("id");
			variable_decl.name("variable_decl");
			variables_decl.name("variables_decl");

			on_error<fail>
			(
			 variables_decl,
			 //variable_decl,
			 std::cout
				<< val("Error! Expecting ")
				<< _4
				<< val(" here: \"")
				<< construct<std::string>(_3, _2)
				<< val("\"")
				<< std::endl
			);
		}

		id_rule;
		variable_decl_rule;
		qi::rule<Iterator, VariablesDecl(), ascii::space_type> variables_decl;
	};

	//-------------------------------------------------------------------
	bool VariablesDeclParser_parse(std::string input_str, client::VariablesDecl& parsed_output)
	{
		client::VariablesDeclParser<iterator_type> grammar;
		iterator_type start(input_str.begin());
		iterator_type end(input_str.end());

		bool result(phrase_parse(start, end, grammar, boost::spirit::ascii::space, parsed_output));
		//return result && start == end;
		if( result && start == end )
		{
			return true;
		}
		else
		{
			std::cout << "fail!!!\n";
			return false;
		}
	}


	//-------------------------------------------------------------------
	// ProgramParser
	//-------------------------------------------------------------------
	template <typename Iterator>
	//struct program_parser : qi::grammar<Iterator, Program(), ascii::space_type>
	struct program_parser : qi::grammar<Iterator, std::string(), ascii::space_type>
	{
		//program_parser() : program_parser::base_type(start)
		program_parser() : program_parser::base_type(id)
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

			id %= lexeme[+(alpha) > *(alnum)];

			//var_decl %= lit("VAR") > lexeme["var1"] > lexeme[":"] > lexeme["BOOL"] > lexeme["END_VAR"];
			//start %=
			//	lit("PROGRAM")
			//	//>> *(char_)
			//	//>> lexeme["aaa"]						// success
			//	//>> lexeme[char_ >> char_ >> char_]	// success
			//	> lexeme[(alpha) > *(alnum)]	// success
			//	> lexeme[*(var_decl)]
			//	> lexeme["END_PROGRAM"]
			//	;

			//start.name("start");

			//on_error<fail>
			//	(
			//	 //start,
			//	 id,
			//	 std::cout
			//	 	<< val("Error! Expecting ")
			//		<< _4
			//		<< val(" here: \"")
			//		<< construct<std::string>(_3, _2)
			//		<< val("\"")
			//		<< std::endl
			//	);
		}

		qi::rule<Iterator, std::string(), ascii::space_type> id;

		//qi::rule<Iterator, Variable(), ascii::space_type> var_decl;
		//qi::rule<Iterator, Program(), ascii::space_type> start;
	};
}

//-------------------------------------------------------------------
// Interpreter
//-------------------------------------------------------------------
Interpreter::Interpreter()
{
}

//-------------------------------------------------------------------
Interpreter::~Interpreter()
{
	if(program_ != nullptr)
	{
		delete program_;
	}
}

void Interpreter::interpret(
	std::string str
	)
{
	typedef std::string::const_iterator iterator_type;
	typedef client::program_parser<iterator_type> program_parser;

	program_parser g; // our grammar

	std::string::const_iterator start = str.begin();
	std::string::const_iterator end = str.end();

	client::Program prg;
	std::string s;

	bool r = phrase_parse(start, end, g, boost::spirit::ascii::space, s);
	if(r && start == end)
	{
		//std::cout << "succeeded\n";
		//std::cout << boost::fusion::as_vector(s) << std::endl;
		std::cout << s << std::endl;

		//program_ = new iec::Program(s.name);
		program_ = new iec::Program(s);
	}
	else
	{
		std::cout << "failed\n";
	}
}

iec::Program* Interpreter::getProgram()
{
	return program_;
}

