#include "Class.Command.hpp"
#include "Class.IRC.hpp"

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

/*
** ==================================================
** getCommand - возвращает текущую команду из буфера
** ==================================================
*/

const string& Command::
getCommand() const			{ return (_command); }

const string &Command::
getMessage() const			{ return (_message); }

const string &Command::
getPrefix() const			{ return (_prefix); }

const vector<string>& Command::
getArgs() const				{ return _arguments; }

bool Command::
check_args_number(int n) const
{
	if ((int)_arguments.size() != n)
		return false;
	return true;
}

bool Command::
check_password(const Client& client) const
{
	if (client.getPassword() == false)
	{
		utils::print_error(123, "ENTER PASS <password>");
		return false;
	}
	return true;
}

bool Command::
check_nickname(const Client& client) const
{
	if (client.getName().empty())
	{
		utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
		return false;
	}
	return true;
}

bool Command::
is_server(IRC& irc, int fd)			{ return irc.is_server(fd); }

User* Command::
find_user(user_map& users, int fd)
{
	for (user_iterator it = users.begin(); it != users.end(); it++)
	{
		if (it->first->getSocketFd() == fd)
			return it->first;
	}
	return NULL;
}
