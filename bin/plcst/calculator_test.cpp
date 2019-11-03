#include <iostream>
#include <tuple>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "calculator.hpp"

//-------------------------------------------------------------------
// Calculator_test
//-------------------------------------------------------------------
class Calculator_test : public ::testing::TestWithParam<
			std::tuple< std::string, std::string >
		>
{ 
};

//-------------------------------------------------------------------
TEST_P(Calculator_test, _)
{
	std::string input(std::get<0>(GetParam()));
	std::string expected_output(std::get<1>(GetParam()));
	std::string output;

	bool result(Calculator::calc(input, output));

	EXPECT_TRUE(result);
	EXPECT_EQ(expected_output, output);
}

//-------------------------------------------------------------------
INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	Calculator_test,
	testing::Values(
		//			input,					expected_output
		make_tuple(	std::string("1+1"),		std::string("2")		),
		make_tuple(	std::string("1+2"),		std::string("3")		)
	)
);

