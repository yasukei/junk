#include <iostream>
#include <tuple>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "interpreter.hpp"
#include "iec.hpp"

#define cPROGRAM		("PROGRAM")
#define cEND_PROGRAM	("END_PROGRAM")

//-------------------------------------------------------------------
// ConcatString
//-------------------------------------------------------------------
class ConcatString
{
public:
	ConcatString(std::string delimiter) :
		str_(""),
		delimiter_(delimiter)
	{
	}

	void operator+=(const std::string str)
	{
		str_ += delimiter_ + str;
	}

	std::string getString()
	{
		return str_;
	}

private:
	std::string str_;
	std::string delimiter_;
};

//-------------------------------------------------------------------
// IdParser_test
//-------------------------------------------------------------------
class IdParser_test : public ::testing::TestWithParam<
			std::tuple< std::string, bool >
		>
{ 
};

//-------------------------------------------------------------------
TEST_P(IdParser_test, _)
{
	std::string input(std::get<0>(GetParam()));
	bool expected_result(std::get<1>(GetParam()));
	std::string output;

	bool actual_result(client::IdParser_parse(input, output));

	EXPECT_EQ(expected_result, actual_result);
	if(actual_result)
	{
		EXPECT_EQ(input, output);
	}
}

//-------------------------------------------------------------------
INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	IdParser_test,
	testing::Values(
		//			input,					expected_result
		make_tuple(	std::string("a"),		bool(true)		),
		make_tuple(	std::string("z"),		bool(true)		),
		make_tuple(	std::string("A"),		bool(true)		),
		make_tuple(	std::string("Z"),		bool(true)		),
		make_tuple(	std::string("aa"),		bool(true)		),
		make_tuple(	std::string("zz"),		bool(true)		),
		make_tuple(	std::string("AA"),		bool(true)		),
		make_tuple(	std::string("ZZ"),		bool(true)		),
		make_tuple(	std::string("a0"),		bool(true)		),
		make_tuple(	std::string("z9"),		bool(true)		),
		make_tuple(	std::string("A0"),		bool(true)		),
		make_tuple(	std::string("Z9"),		bool(true)		),
		make_tuple(	std::string("0"),		bool(false)		),
		make_tuple(	std::string("9"),		bool(false)		),
		make_tuple(	std::string("00"),		bool(false)		),
		make_tuple(	std::string("99"),		bool(false)		),
		make_tuple(	std::string("/"),		bool(false)		),
		make_tuple(	std::string("#"),		bool(false)		)
	)
);

//-------------------------------------------------------------------
// VariableDeclParser_test
//-------------------------------------------------------------------
class VariableDeclParser_test : public ::testing::TestWithParam<
			std::tuple< std::string, bool, std::string, std::string >
		>
{ 
};

//-------------------------------------------------------------------
TEST_P(VariableDeclParser_test, _)
{
	std::string input(std::get<0>(GetParam()));
	bool expected_result(std::get<1>(GetParam()));
	std::string expected_var_name(std::get<2>(GetParam()));
	std::string expected_type_name(std::get<3>(GetParam()));
	client::VariableDecl output;

	bool actual_result(client::VariableDeclParser_parse(input, output));

	EXPECT_EQ(expected_result, actual_result);
	if(actual_result)
	{
		EXPECT_EQ(expected_var_name, output.var_name);
		EXPECT_EQ(expected_type_name, output.type_name);
	}
}

//-------------------------------------------------------------------
INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	VariableDeclParser_test,
	testing::Values(
		//			input,				
		//									expected_result,	expected_var_name,		expected_type_name
		make_tuple(	std::string("var : type"),	
											bool(true),			std::string("var"),		std::string("type")		),

		make_tuple(	std::string("a : b"),	
											bool(true),			std::string("a"),		std::string("b")		)
	)
);

//-------------------------------------------------------------------
// VariablesDeclParser_test
//-------------------------------------------------------------------
class VariablesDeclParser_test : public ::testing::TestWithParam<
			std::tuple< std::string, bool, std::set<client::VariableDecl> >
		>
{ 
};

//-------------------------------------------------------------------
TEST_P(VariablesDeclParser_test, _)
{
	auto input(std::get<0>(GetParam()));
	auto expected_result(std::get<1>(GetParam()));
	auto expected_set(std::get<2>(GetParam()));
	client::VariablesDecl output;

	bool actual_result(client::VariablesDeclParser_parse(input, output));

	EXPECT_EQ(expected_result, actual_result);
	if(actual_result)
	{
		EXPECT_EQ(expected_set.size(), output.vars_decl.size());
		EXPECT_TRUE(expected_set == output.vars_decl);
	}
}

//-------------------------------------------------------------------
INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	VariablesDeclParser_test,
	testing::Values(
		//			input,				
		//									expected_result,	expected_set,
		make_tuple(	std::string("VAR var : type VAR_END"),	
											bool(true),			std::set<client::VariableDecl>({client::VariableDecl("var", "type")})	)
		)
);

//	const std::string program_name = R"END(
//PROGRAM program1
//	VAR
//		var1	: BOOL;
//	END_VAR
//END_PROGRAM
//)END";

//-------------------------------------------------------------------
TEST(Program, single)
{
	Interpreter itp;
	ConcatString str(" ");
	const std::string program_name("myprogram1");

	//str += cPROGRAM;
	str += program_name;
	//str += cEND_PROGRAM;

	//std::string str("PROGRAM a1a END_PROGRAM");
	//std::string str("PROGRAM a1a VAR x : INT ; VAR_END END_PROGRAM");

	itp.interpret(str.getString());
	iec::Program* p(itp.getProgram());

	EXPECT_EQ(program_name, p->getName());

}

//-------------------------------------------------------------------
//TEST(Variable, single)
//{
//	Interpreter itp;
//	ConcatString str(" ");
//	//const std::string program_name("myprogram1");
//	const std::string program_name = R"END(
//PROGRAM program1
//	VAR
//		var1	: BOOL;
//	END_VAR
//END_PROGRAM
//)END";
//
//	str += program_name;
//
//	itp.interpret(str.getString());
//	iec::Program* p(itp.getProgram());
//	//iec::Variable* v(p->getVariable());
//
//	EXPECT_EQ("program1", p->getName());
//
//}

