#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include <string>
#include <iostream>
using namespace std;

// ------------------------------------------------------------------
// Command
// ------------------------------------------------------------------
class Command
{
	public:
		~Command(void) {};

		virtual bool execute(const string& arg, ostream& os) = 0;
};

// ------------------------------------------------------------------
#define cCommand_create_suffix	"_create"
#define cCommand_destroy_suffix	"_destroy"
typedef Command* (*Command_create)(void);
typedef void (*Command_destroy)(Command* command);

// ------------------------------------------------------------------
// Commands
// ------------------------------------------------------------------
class Commands
{
	public:
		static Commands* create(ostream& os);
		~Commands(void) {};

		virtual bool register_command_library(const string& path_to_library) = 0;
		virtual bool execute_command(const string& command_name, const string& arg) = 0;

		// copyConstructor
		//Commands(const Commands& rhs);
		// substitution operator
		//Commands& operator=(const Commands& rhs);
};

// ------------------------------------------------------------------
// CommandsUtil
// ------------------------------------------------------------------
class CommandsUtil
{
	public:
		static void parse(istream& is, string& command_name, string& arg);
};

// ------------------------------------------------------------------
// StringUtil
// ------------------------------------------------------------------
class StringUtil
{
	public:
		static string::size_type get_indexof_xth_word_head(
			const string& str,
			const unsigned int xth,
			const string& delims = " \t"
			);
		static string::size_type get_indexof_xth_word_tail(
			const string& str,
			const unsigned int xth,
			const string& delims = " \t"
			);
		static string& get_xth_word(
			const string& str,
			const unsigned int xth,
			string& word,
			const string& delims = " \t"
			);
		static string& trim_delims_on_head(
			string& str,
			const string& delims = " \t"
			);
		static string& trim_delims_on_tail(
			string& str,
			const string& delims = " \t"
			);
};

#endif /* COMMANDS_HPP_ */
