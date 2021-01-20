#include "Class.Command.hpp"
#include "Class.IRC.hpp"

using std::vector;
using std::string;

/*
** ==========================================
** конструктор и деструктор Command()
** ==========================================
*/

Command::Command(const string& str) : message(str)
{
	string				last_arg;
	char *				tok;
	size_t				last_pos;
	char *char_str = new char[str.length() + 1];

	strcpy(char_str, str.c_str());
	tok = strtok(char_str, " ");

	if (tok[0] == ':')
	{
		prefix = static_cast<std::string>(&tok[1]);
		tok = strtok(NULL, " ");
		command = tok == NULL ? "" : static_cast<std::string>(tok);
	}
	else
	{
		prefix = "";
		command = static_cast<std::string>(tok);
	}
	std::cout << "Prefix: " << prefix << std::endl
			  << "Command: " << command << std::endl; // DEBUG
	while (tok != NULL && (tok = strtok(NULL, " ")) != NULL)
	{
		if (tok[0] == ':')
			break;
		this->arguments.push_back(static_cast<std::string>(tok));
		std::cout << "Argument: " << tok << std::endl; // DEBUG
	}

	if ((last_pos = str.find(" :")) != std::string::npos)
	{
		this->arguments.push_back(str.substr(last_pos + 2, str.size() - last_pos - 2));
		std::cout << "Argument: " << str.substr(last_pos + 2, str.size() - last_pos - 2) << std::endl; // DEBUG
	}

	delete [] char_str;
}

Command::~Command() {}

/*
** =====================================================================
** find_fd - находит в векторе итераторную позицию которая соответствует 
**           переданный fd
** =====================================================================
*/

vector<Client*>::iterator Command::
find_fd(vector<Client*>& vect, int fd)
{
	vector<Client*>::iterator v_begin	= vect.begin();

	while (v_begin != vect.end())
	{
		if ((*v_begin)->getSocketFd() == fd)
			return (v_begin);
		v_begin++;
	}
	return (vect.end());
}

/*
** ==================================================
** getCommand - возвращает текущую команду из буфера
** ==================================================
*/

const string& Command::
getCommand() const			{ return (this->command); }

const string &Command::
getMessage() const			{ return (this->message); }

const string &Command::
getPrefix() const			{ return (this->prefix); }

const vector<string>& Command::
getArgs() const				{ return this->arguments; }

bool Command::
check_args_number(int n) const
{
	if ((int)this->arguments.size() != n)
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