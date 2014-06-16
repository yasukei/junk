#include <dlfcn.h>
#include <vector>
using namespace std;

#include "Commands.hpp"
#include "private.h"

// ------------------------------------------------------------------
// define
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// CommandsImpl
// ------------------------------------------------------------------
class CommandsImpl : public Commands
{
	public:
		CommandsImpl(ostream& output_stream);
		~CommandsImpl(void);

		bool register_command_library(const string& path_to_library);
		bool execute_command(const string& command_name, const string& arg);

	protected:
		ostream& output_stream_;
		vector<void*> library_handles_;

		void* find_symbol(const string& symbol_name);
		Command_create find_Command_create(const string& command_name);
		Command_destroy find_Command_destroy(const string& command_name);
};

// ------------------------------------------------------------------
CommandsImpl::CommandsImpl(
	ostream& output_stream
	) :
	output_stream_(output_stream),
	library_handles_()
{
}

// ------------------------------------------------------------------
CommandsImpl::~CommandsImpl(void)
{
	vector<void*>::iterator lib_handle;
	for(lib_handle = library_handles_.begin(); lib_handle != library_handles_.end(); lib_handle++)
	{
		  dlclose(*lib_handle);
	}
}

// ------------------------------------------------------------------
bool CommandsImpl::register_command_library(
	const string& path_to_library
	)
{
	int flag = RTLD_NOW | RTLD_LOCAL;
	void* lib_handle;

	dlerror();
	lib_handle = dlopen(path_to_library.c_str(), flag);
	if(lib_handle == NULL)
	{
		cerr << dlerror() << endl;
		return false;
	}

	library_handles_.push_back(lib_handle);
	return true;
}

// ------------------------------------------------------------------
void* CommandsImpl::find_symbol(
	const string& symbol_name
	)
{
	vector<void*>::iterator lib_handle;
	void* symbol;

	for(lib_handle = library_handles_.begin(); lib_handle != library_handles_.end(); lib_handle++)
	{
		symbol = dlsym(*lib_handle, symbol_name.c_str());
		if(symbol != NULL)
		{
			return symbol;
		}
	}
	return NULL;
}

// ------------------------------------------------------------------
Command_create CommandsImpl::find_Command_create(
	const string& command_name
	)
{
	return (Command_create)find_symbol(command_name + cCommand_create_suffix);
}

// ------------------------------------------------------------------
Command_destroy CommandsImpl::find_Command_destroy(
	const string& command_name
	)
{
	return (Command_destroy)find_symbol(command_name + cCommand_destroy_suffix);
}

// ------------------------------------------------------------------
bool CommandsImpl::execute_command(
	const string& command_name,
	const string& arg
	)
{
	Command_create create_method;
	Command_destroy destroy_method;
	Command* command;

	create_method = find_Command_create(command_name);
	destroy_method = find_Command_destroy(command_name);
	if( (create_method == NULL) || (destroy_method == NULL) )
	{
		CHECKPOINT;
		return false;
	}

	command = (*create_method)();
	if(command == NULL)
	{
		CHECKPOINT;
		return false;
	}

	command->execute(arg, output_stream_);

	(*destroy_method)(command);

	return true;
}

// ------------------------------------------------------------------
// Commands
// ------------------------------------------------------------------
Commands* Commands::create(ostream& output_stream)
{
	return new CommandsImpl(output_stream);
}

// ------------------------------------------------------------------
Commands::~Commands(void)
{
}

// ------------------------------------------------------------------
// CommandsUtil
// ------------------------------------------------------------------
void CommandsUtil::parse(istream& is, string& command_name, string& arg)
{
	string one_line;

	std::getline(is, one_line);

	StringUtil::take_xth_word(one_line, 1, command_name);
	StringUtil::trim_delims_on_head(one_line);
	StringUtil::trim_delims_on_tail(one_line);
	arg.assign(one_line);
}

// ------------------------------------------------------------------
// StringUtil
// ------------------------------------------------------------------
string::size_type StringUtil::get_indexof_xth_word_head(
	const string& str,
	const unsigned int xth,
	const string& delims
	)
{
	string::size_type word_head_index, delim_head_index;
	unsigned int ith;

	word_head_index = string::npos;
	delim_head_index = 0;
	for(ith = 0; ith < xth; ith++)
	{
		word_head_index = str.find_first_not_of(delims, delim_head_index);
		if(word_head_index == string::npos)
		{
			return string::npos;
		}
		delim_head_index = str.find_first_of(delims, word_head_index);
	}

	return word_head_index;
}

// ------------------------------------------------------------------
string::size_type StringUtil::get_indexof_xth_word_tail(
	const string& str,
	const unsigned int xth,
	const string& delims
	)
{
	string::size_type word_head_index;
	string::size_type delim_head_index;
	
	word_head_index = StringUtil::get_indexof_xth_word_head(str, xth, delims);
	if(word_head_index == string::npos)
	{
		return string::npos;
	}
	delim_head_index = str.find_first_of(delims, word_head_index);
	if(delim_head_index == string::npos)
	{
		return str.length() - 1;
	}
	return delim_head_index - 1;
}

// ------------------------------------------------------------------
string& StringUtil::get_xth_word(
	const string& str,
	const unsigned int xth,
	string& word,
	const string& delims
	)
{
	string::size_type word_head_index, word_tail_index;

	word_head_index = StringUtil::get_indexof_xth_word_head(str, xth, delims);
	if(word_head_index == string::npos)
	{
		return word.assign("");
	}
	word_tail_index = StringUtil::get_indexof_xth_word_tail(str, xth, delims);

	return word.assign( str.substr(word_head_index, word_tail_index - word_head_index + 1) );
}

// ------------------------------------------------------------------
string& StringUtil::take_xth_word(
	string& str,
	const unsigned int xth,
	string& word,
	const string& delims
	)
{
	string::size_type word_head_index, word_tail_index;

	word_head_index = StringUtil::get_indexof_xth_word_head(str, xth, delims);
	if(word_head_index == string::npos)
	{
		return word.assign("");
	}
	word_tail_index = StringUtil::get_indexof_xth_word_tail(str, xth, delims);

	word.assign( str.substr(word_head_index, word_tail_index - word_head_index + 1) );
	str.erase(word_head_index, word_tail_index - word_head_index + 1);
	return word;
}

// ------------------------------------------------------------------
string& StringUtil::trim_delims_on_head(
	string& str,
	const string& delims
	)
{
	string::size_type word_head_index;

	word_head_index = str.find_first_not_of(delims);
	if(word_head_index == 0)
	{
		return str;
	}
	if(word_head_index == string::npos)
	{
		return str.erase();
	}

	return str.erase(0, word_head_index);
}

// ------------------------------------------------------------------
string& StringUtil::trim_delims_on_tail(
	string& str,
	const string& delims
	)
{
	string::size_type word_tail_index;

	word_tail_index = str.find_last_not_of(delims);
	if(word_tail_index == (str.length() - 1) )
	{
		return str;
	}
	if(word_tail_index == string::npos)
	{
		return str.erase();
	}

	return str.erase(word_tail_index + 1, str.length() - word_tail_index);
}

