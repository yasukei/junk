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

	lib_handle = dlopen(path_to_library.c_str(), flag);
	if(lib_handle == NULL)
	{
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

