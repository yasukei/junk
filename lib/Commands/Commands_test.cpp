// test target
#include "Commands.hpp"
#include "private.h"

// helper
//using std::vector;

// test framework
#include "gtest/gtest.h"

// ------------------------------------------------------------------
// Constant
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper class
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Test_Commands_register_command_library
// ------------------------------------------------------------------
class Test_Commands_register_command_library : public ::testing::Test
{ 
	protected:
		Commands* commands_;

		virtual void SetUp(void)
		{
			commands_ = Commands::create(cout);
			ASSERT_NE((Commands*)NULL, commands_);
		}
		virtual void TearDown(void)
		{
			delete(commands_);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_Commands_register_command_library, success)
{
	// precondition

	// target
	bool ret = commands_->register_command_library("./libTestCommands.so");

	// postcondition
	EXPECT_TRUE(ret);
}

// ------------------------------------------------------------------
// Test_Commands_execute_command
// ------------------------------------------------------------------
class Test_Commands_execute_command : public ::testing::Test
{ 
	protected:
		Commands* commands_;

		virtual void SetUp(void)
		{
			commands_ = Commands::create(cout);
			ASSERT_NE((Commands*)NULL, commands_);
			ASSERT_TRUE(commands_->register_command_library("./libTestCommands.so"));
		}
		virtual void TearDown(void)
		{
			delete(commands_);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_Commands_execute_command, success)
{
	// precondition

	// target
	bool ret = commands_->execute_command("TestCommand1", "This is arguments!");

	// postcondition
	EXPECT_TRUE(ret);
}

// ------------------------------------------------------------------
// Test_CommandsUtil
// ------------------------------------------------------------------
class Test_CommandsUtil_parse : public ::testing::TestWithParam< vector<string> >
{ 
};

TEST_P(Test_CommandsUtil_parse, various_inputs)
{
	vector<string> param = GetParam();
	stringbuf strbuf(param[0]);
	istream input_stream(&strbuf);
	string command_name;
	string arg;

	// target
	CommandsUtil::parse(input_stream, command_name, arg);

	// postcondition
	//EXPECT_TRUE(ret);
	EXPECT_EQ(param[1], command_name);
	EXPECT_EQ(param[2], arg);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_CommandsUtil_parse,
	testing::Values(
		vector<string> { "\n",						"",			"" },
		vector<string> { " \n",						"",			"" },
		vector<string> { "  \n",					"",			"" },
		vector<string> { "command\n",				"command",	"" },
		vector<string> { " command\n",				"command",	"" },
		vector<string> { "  command\n",				"command",	"" },
		vector<string> { "  command \n",			"command",	"" },
		vector<string> { "  command  \n",			"command",	"" },
		vector<string> { " command  \n",			"command",	"" },
		vector<string> { "command  \n",				"command",	"" },
		vector<string> { "command arg\n",			"command",	"arg" },
		vector<string> { "command  arg\n",			"command",	"arg" },
		vector<string> { "command   arg\n",			"command",	"arg" },
		vector<string> { "command   arg \n",		"command",	"arg" },
		vector<string> { "command   arg  \n",		"command",	"arg" },
		vector<string> { "command  arg  \n",		"command",	"arg" },
		vector<string> { "command arg  \n",			"command",	"arg" },
		vector<string> { "command arg1 arg2\n",		"command",	"arg1 arg2" },
		vector<string> { "command arg1  arg2 \n",	"command",	"arg1  arg2" }
	)
);

// ------------------------------------------------------------------
// Test_StringUtil
// ------------------------------------------------------------------
class Test_StringUtil_get_indexof_xth_word_head : public ::testing::TestWithParam<
	tr1::tuple<string, unsigned int, string::size_type>
	>
{ 
};

TEST_P(Test_StringUtil_get_indexof_xth_word_head, various_inputs)
{
	string::size_type index;

	// target
	index = StringUtil::get_indexof_xth_word_head(
		tr1::get<0>(GetParam()),
		tr1::get<1>(GetParam())
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), index);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_get_indexof_xth_word_head,
	testing::Values(
		tr1::make_tuple( string(""),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(""),		(unsigned int)1, (string::size_type)string::npos ),
		tr1::make_tuple( string(" "),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(" "),		(unsigned int)1, (string::size_type)string::npos ),
		tr1::make_tuple( string("a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a"),		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string(" a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(" a"),		(unsigned int)1, (string::size_type)1 ),
		tr1::make_tuple( string(" a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a"),		(unsigned int)1, (string::size_type)2 ),
		tr1::make_tuple( string("  a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a "),	(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a "),	(unsigned int)1, (string::size_type)2 ),
		tr1::make_tuple( string("  a "),	(unsigned int)2, (string::size_type)string::npos ),
		//
		tr1::make_tuple( string("a b"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a b"),		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a b"),		(unsigned int)2, (string::size_type)2 ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)2, (string::size_type)4 ),
		//
		tr1::make_tuple( string("a b c"), 		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)2, (string::size_type)2 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)3, (string::size_type)4 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)4, (string::size_type)string::npos ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)2, (string::size_type)4 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)3, (string::size_type)8 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)4, (string::size_type)string::npos )
	)
);

// ------------------------------------------------------------------
class Test_StringUtil_get_indexof_xth_word_tail : public ::testing::TestWithParam<
	tr1::tuple<string, unsigned int, string::size_type>
	>
{ 
};

TEST_P(Test_StringUtil_get_indexof_xth_word_tail, various_inputs)
{
	string::size_type index;

	// target
	index = StringUtil::get_indexof_xth_word_tail(
		tr1::get<0>(GetParam()),
		tr1::get<1>(GetParam())
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), index);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_get_indexof_xth_word_tail,
	testing::Values(
		tr1::make_tuple( string(""),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(""),		(unsigned int)1, (string::size_type)string::npos ),
		tr1::make_tuple( string(" "),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(" "),		(unsigned int)1, (string::size_type)string::npos ),
		tr1::make_tuple( string("a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a"),		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string(" a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string(" a"),		(unsigned int)1, (string::size_type)1 ),
		tr1::make_tuple( string(" a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a"),		(unsigned int)1, (string::size_type)2 ),
		tr1::make_tuple( string("  a"),		(unsigned int)2, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a "),	(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("  a "),	(unsigned int)1, (string::size_type)2 ),
		tr1::make_tuple( string("  a "),	(unsigned int)2, (string::size_type)string::npos ),
		//
		tr1::make_tuple( string("a b"),		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a b"),		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a b"),		(unsigned int)2, (string::size_type)2 ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)1, (string::size_type)1 ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)2, (string::size_type)5 ),
		//
		tr1::make_tuple( string("a b c"), 		(unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)1, (string::size_type)0 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)2, (string::size_type)2 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)3, (string::size_type)4 ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)4, (string::size_type)string::npos ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)0, (string::size_type)string::npos ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)1, (string::size_type)2 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)2, (string::size_type)6 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)3, (string::size_type)10 ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)4, (string::size_type)string::npos )
	)
);

// ------------------------------------------------------------------
class Test_StringUtil_get_xth_word : public ::testing::TestWithParam<
	tr1::tuple<string, unsigned int, string>
	>
{ 
};

TEST_P(Test_StringUtil_get_xth_word, various_inputs)
{
	string word;

	// target
	string& ret = StringUtil::get_xth_word(
		tr1::get<0>(GetParam()),
		tr1::get<1>(GetParam()),
		word
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), word);
	EXPECT_EQ(&word, &ret);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_get_xth_word,
	testing::Values(
		tr1::make_tuple( string(""),		(unsigned int)0, string("") ),
		tr1::make_tuple( string(""),		(unsigned int)1, string("") ),
		tr1::make_tuple( string(" "),		(unsigned int)0, string("") ),
		tr1::make_tuple( string(" "),		(unsigned int)1, string("") ),
		tr1::make_tuple( string("a"),		(unsigned int)0, string("") ),
		tr1::make_tuple( string("a"),		(unsigned int)1, string("a") ),
		tr1::make_tuple( string("a"),		(unsigned int)2, string("") ),
		tr1::make_tuple( string(" a"),		(unsigned int)0, string("") ),
		tr1::make_tuple( string(" a"),		(unsigned int)1, string("a") ),
		tr1::make_tuple( string(" a"),		(unsigned int)2, string("") ),
		tr1::make_tuple( string("  a"),		(unsigned int)0, string("") ),
		tr1::make_tuple( string("  a"),		(unsigned int)1, string("a") ),
		tr1::make_tuple( string("  a"),		(unsigned int)2, string("") ),
		tr1::make_tuple( string("  a "),	(unsigned int)0, string("") ),
		tr1::make_tuple( string("  a "),	(unsigned int)1, string("a") ),
		tr1::make_tuple( string("  a "),	(unsigned int)2, string("") ),
		////
		tr1::make_tuple( string("a b"),		(unsigned int)0, string("") ),
		tr1::make_tuple( string("a b"),		(unsigned int)1, string("a") ),
		tr1::make_tuple( string("a b"),		(unsigned int)2, string("b") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)0, string("") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)1, string("aa") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)2, string("bb") ),
		////
		tr1::make_tuple( string("a b c"), 		(unsigned int)0, string("") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)1, string("a") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)2, string("b") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)3, string("c") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)4, string("") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)0, string("") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)1, string("aaa") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)2, string("bbb") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)3, string("ccc") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)4, string("") )
	)
);

// ------------------------------------------------------------------
class Test_StringUtil_take_xth_word : public ::testing::TestWithParam<
	tr1::tuple<string, unsigned int, string, string>
	>
{ 
};

TEST_P(Test_StringUtil_take_xth_word, various_inputs)
{
	string str = tr1::get<0>(GetParam());
	unsigned int xth = tr1::get<1>(GetParam());
	string word;

	// target
	string& ret = StringUtil::take_xth_word(
		str,
		xth,
		word
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), str);
	EXPECT_EQ(tr1::get<3>(GetParam()), word);
	EXPECT_EQ(&word, &ret);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_take_xth_word,
	testing::Values(
		tr1::make_tuple( string(""),		(unsigned int)0, string(""), string("") ),
		tr1::make_tuple( string(""),		(unsigned int)1, string(""), string("") ),
		tr1::make_tuple( string(" "),		(unsigned int)0, string(" "), string("") ),
		tr1::make_tuple( string(" "),		(unsigned int)1, string(" "), string("") ),
		tr1::make_tuple( string("a"),		(unsigned int)0, string("a"), string("") ),
		tr1::make_tuple( string("a"),		(unsigned int)1, string(""), string("a") ),
		tr1::make_tuple( string("a"),		(unsigned int)2, string("a"), string("") ),
		tr1::make_tuple( string(" a"),		(unsigned int)0, string(" a"), string("") ),
		tr1::make_tuple( string(" a"),		(unsigned int)1, string(" "), string("a") ),
		tr1::make_tuple( string(" a"),		(unsigned int)2, string(" a"), string("") ),
		tr1::make_tuple( string("  a"),		(unsigned int)0, string("  a"), string("") ),
		tr1::make_tuple( string("  a"),		(unsigned int)1, string("  "), string("a") ),
		tr1::make_tuple( string("  a"),		(unsigned int)2, string("  a"), string("") ),
		tr1::make_tuple( string("  a "),	(unsigned int)0, string("  a "), string("") ),
		tr1::make_tuple( string("  a "),	(unsigned int)1, string("   "), string("a") ),
		tr1::make_tuple( string("  a "),	(unsigned int)2, string("  a "), string("") ),
		//
		tr1::make_tuple( string("a b"),		(unsigned int)0, string("a b"),		string("") ),
		tr1::make_tuple( string("a b"),		(unsigned int)1, string(" b"),		string("a") ),
		tr1::make_tuple( string("a b"),		(unsigned int)2, string("a "),		string("b") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)0, string("aa  bb"),	string("") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)1, string("  bb"),	string("aa") ),
		tr1::make_tuple( string("aa  bb"),	(unsigned int)2, string("aa  "),	string("bb") ),
		//
		tr1::make_tuple( string("a b c"), 		(unsigned int)0, string("a b c"),		string("") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)1, string(" b c"),		string("a") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)2, string("a  c"),		string("b") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)3, string("a b "),		string("c") ),
		tr1::make_tuple( string("a b c"), 		(unsigned int)4, string("a b c"),		string("") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)0, string("aaa bbb ccc"),	string("") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)1, string(" bbb ccc"),	string("aaa") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)2, string("aaa  ccc"),	string("bbb") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)3, string("aaa bbb "),	string("ccc") ),
		tr1::make_tuple( string("aaa bbb ccc"), (unsigned int)4, string("aaa bbb ccc"),	string("") )
	)
);

// ------------------------------------------------------------------
class Test_StringUtil_trim_delims_on_head : public ::testing::TestWithParam<
	tr1::tuple<string, string, string>
	>
{ 
};

TEST_P(Test_StringUtil_trim_delims_on_head, various_inputs)
{
	string str = tr1::get<0>(GetParam());

	// target
	string& ret = StringUtil::trim_delims_on_head(
		str,
		tr1::get<1>(GetParam())
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), str);
	EXPECT_EQ(&str, &ret);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_trim_delims_on_head,
	testing::Values(
		tr1::make_tuple( string(""),		string(""),		string("") ),
		tr1::make_tuple( string(" "),		string(""),		string(" ") ),
		tr1::make_tuple( string(" "),		string(" "),	string("") ),
		tr1::make_tuple( string(""),		string(" "),	string("") ),
		tr1::make_tuple( string("a"),		string(""),		string("a") ),
		tr1::make_tuple( string("a"),		string(" "),	string("a") ),
		tr1::make_tuple( string(" a"),		string(" "),	string("a") ),
		tr1::make_tuple( string("  a"),		string(" "),	string("a") ),
		tr1::make_tuple( string("  a "),	string(" "),	string("a ") ),
		tr1::make_tuple( string("  a  "),	string(" "),	string("a  ") ),
		tr1::make_tuple( string(" a  "),	string(" "),	string("a  ") ),
		tr1::make_tuple( string("a  "),		string(" "),	string("a  ") ),
		tr1::make_tuple( string(" \ta"),	string(" \t"),	string("a") ),
		tr1::make_tuple( string("\t a"),	string(" \t"),	string("a") ),
		tr1::make_tuple( string(" \ta"),	string(" \t"),	string("a") ),
		tr1::make_tuple( string("\t a"),	string(" \t"),	string("a") )
	)
);

// ------------------------------------------------------------------
class Test_StringUtil_trim_delims_on_tail : public ::testing::TestWithParam<
	tr1::tuple<string, string, string>
	>
{ 
};

TEST_P(Test_StringUtil_trim_delims_on_tail, various_inputs)
{
	string str = tr1::get<0>(GetParam());

	// target
	string& ret = StringUtil::trim_delims_on_tail(
		str,
		tr1::get<1>(GetParam())
		);

	// postcondition
	EXPECT_EQ(tr1::get<2>(GetParam()), str);
	EXPECT_EQ(&str, &ret);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Test_StringUtil_trim_delims_on_tail,
	testing::Values(
		tr1::make_tuple( string(""),		string(""),		string("") ),
		tr1::make_tuple( string(" "),		string(""),		string(" ") ),
		tr1::make_tuple( string(" "),		string(" "),	string("") ),
		tr1::make_tuple( string(""),		string(" "),	string("") ),
		tr1::make_tuple( string("a"),		string(""),		string("a") ),
		tr1::make_tuple( string("a"),		string(" "),	string("a") ),
		tr1::make_tuple( string(" a"),		string(" "),	string(" a") ),
		tr1::make_tuple( string("  a"),		string(" "),	string("  a") ),
		tr1::make_tuple( string("  a "),	string(" "),	string("  a") ),
		tr1::make_tuple( string("  a  "),	string(" "),	string("  a") ),
		tr1::make_tuple( string(" a  "),	string(" "),	string(" a") ),
		tr1::make_tuple( string("a  "),		string(" "),	string("a") ),
		tr1::make_tuple( string("a \t"),	string(" \t"),	string("a") ),
		tr1::make_tuple( string("a\t "),	string(" \t"),	string("a") ),
		tr1::make_tuple( string("a \t"),	string(" \t"),	string("a") ),
		tr1::make_tuple( string("a\t "),	string(" \t"),	string("a") )
	)
);

