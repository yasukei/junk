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
		Commands* commands;

		virtual void SetUp(void)
		{
			commands = Commands::create(cout);
			ASSERT_NE((Commands*)NULL, commands);
		}
		virtual void TearDown(void)
		{
			delete(commands);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_Commands_register_command_library, success)
{
	// precondition

	// target
	bool ret = commands->register_command_library("./libTestCommands.so");

	// postcondition
	EXPECT_TRUE(ret);
}

// ------------------------------------------------------------------
// Test_Commands_execute_command
// ------------------------------------------------------------------
class Test_Commands_execute_command : public ::testing::Test
{ 
	protected:
		Commands* commands;

		virtual void SetUp(void)
		{
			commands = Commands::create(cout);
			ASSERT_NE((Commands*)NULL, commands);
			ASSERT_TRUE(commands->register_command_library("./libTestCommands.so"));
		}
		virtual void TearDown(void)
		{
			delete(commands);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_Commands_execute_command, success)
{
	// precondition

	// target
	bool ret = commands->execute_command("TestCommand1", "This is arguments!");

	// postcondition
	EXPECT_TRUE(ret);
}

