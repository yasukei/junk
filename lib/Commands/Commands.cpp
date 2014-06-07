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
		CommandsImpl(ostream& os);
		~CommandsImpl(void);

		bool register_command_library(const string& path_to_library);
		bool execute_command(const string& command_name, const string& arg);

	protected:
		ostream& _os;
		vector<void*> _lib_handles;

		void* _find_symbol(const string& symbol_name);
		Command_create _find_Command_create(const string& command_name);
		Command_destroy _find_Command_destroy(const string& command_name);
};

// ------------------------------------------------------------------
CommandsImpl::CommandsImpl(
	ostream& os
	) :
	_os(os),
	_lib_handles()
{
}

// ------------------------------------------------------------------
CommandsImpl::~CommandsImpl(void)
{
	vector<void*>::iterator lib_handle;
	for(lib_handle = _lib_handles.begin(); lib_handle != _lib_handles.end(); lib_handle++)
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

	_lib_handles.push_back(lib_handle);
	return true;
}

// ------------------------------------------------------------------
void* CommandsImpl::_find_symbol(
	const string& symbol_name
	)
{
	vector<void*>::iterator lib_handle;
	void* symbol;

	for(lib_handle = _lib_handles.begin(); lib_handle != _lib_handles.end(); lib_handle++)
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
Command_create CommandsImpl::_find_Command_create(
	const string& command_name
	)
{
	return (Command_create)_find_symbol(command_name + cCommand_create_suffix);
}

// ------------------------------------------------------------------
Command_destroy CommandsImpl::_find_Command_destroy(
	const string& command_name
	)
{
	return (Command_destroy)_find_symbol(command_name + cCommand_destroy_suffix);
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

	create_method = _find_Command_create(command_name);
	destroy_method = _find_Command_destroy(command_name);
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

	command->execute(arg, _os);

	(*destroy_method)(command);

	return true;
}

// ------------------------------------------------------------------
// Commands
// ------------------------------------------------------------------
Commands* Commands::create(ostream& os)
{
	return new CommandsImpl(os);
}

// ------------------------------------------------------------------
Commands::~Commands(void)
{
}

// ------------------------------------------------------------------
// CommandsUtil
// ------------------------------------------------------------------
/*
void __get_first_word(const string& str, string& word)
{
	string::size_type command_head, command_tail;

	command_head = one_line.find_first_not_of(delims);
	if(command_head == string::npos)
	{
		return;
	}
	command_tail = one_line.find_first_of(delims, command_head);
	if(command_tail == string::npos)
	{
		command_name.assign( one_line.substr(command_head) );
		return;
	}
	command_name.assign( one_line.substr(command_head, command_tail - command_head) );
}

// ------------------------------------------------------------------
string& __remove_delims_on_head(const string& str, const string& delims = " \t")
{
	string::size_type word_head = __get_indexof_xth_word_head(str);

	if(word_head == 0)
	{
		return str;
	}
	else
	{
		return str.erase(0, word_head - 1);
	}
}
*/

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

