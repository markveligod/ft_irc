#include "Class.Command.hpp"

using std::vector;
using std::string;

/*
** ==========================================
** конструктор и деструктор Command()
** ==========================================
*/

Command::Command(const string& str) : _message(str)
{
	string				last_arg;
	char *				tok;
	size_t				last_pos;
	char *char_str = new char[str.length() + 1];

	strcpy(char_str, str.c_str());
	tok = strtok(char_str, " ");

	if (tok[0] == ':')
	{
		_prefix = static_cast<std::string>(&tok[1]);
		tok = strtok(NULL, " ");
		_command = tok == NULL ? "" : static_cast<std::string>(tok);
	}
	else
	{
		_prefix = "";
		_command = static_cast<std::string>(tok);
	}
	while (tok != NULL && (tok = strtok(NULL, " ")) != NULL)
	{
		if (tok[0] == ':')
			break;
		_arguments.push_back(static_cast<std::string>(tok));
	}

	if ((last_pos = str.find(" :")) != std::string::npos)
		_arguments.push_back(str.substr(last_pos + 2, str.size() - last_pos - 2));

	delete [] char_str;
}

Command::~Command() {}

const string& Command::
getCommand() const			{ return (_command); }

const string &Command::
getMessage() const			{ return (_message); }

const string &Command::
getPrefix() const			{ return (_prefix); }

const vector<string>& Command::
getArgs() const				{ return _arguments; }
