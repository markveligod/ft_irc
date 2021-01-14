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
	std::istringstream	ss(str);
	string				word;
	string				last_arg;

	getline(ss, word, ' ');
	if (word[0] == ':')
	{
		this->prefix = word.substr(1, word.length());
		getline(ss, this->command, ' ');
	}
	else
	{
		this->prefix = string();
		this->command = word;
	}

	while (getline(ss, word, ' '))
	{
		if (word[0] == ':')
		{
			last_arg = word.substr(1, word.length());
			break;
		}
		this->arguments.push_back(word);
	}
	while (getline(ss, word, ' '))
		last_arg = last_arg + ' ' + word;
	if (!(last_arg.empty()))
	this->arguments.push_back(last_arg);
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
getCommand() const { return (this->command); }

const string &Command::
getMessage() const { return (this->message); }

bool Command::check_args_number(int n) const
{
	if ((int)this->arguments.size() < n)
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
	if (client.getNickname().empty())
	{
		utils::print_error(ERR_NICKNAME, "ENTER NICK <nickname>");
		return false;
	}
	return true;
}

bool Command::
is_channel_visible(IRC& irc, int fd, char channel_type, const string& channel_name)
{
	map<string, Channel>& channels = (channel_type == '&')
									? irc.get_local_channels()
									: irc.get_shared_channels();
	
	map<string, Channel>::iterator it = channels.find(channel_name);
	if (it == channels.end())								// проверяем, что канал существует
		return false;

	size_t i = IRC::find_fd(it->second.get_users(), fd);	// it->second - канал
	return i >=0 || it->second.is_visible();
}
