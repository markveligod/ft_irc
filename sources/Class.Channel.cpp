#include "Class.Channel.hpp"

Channel::Channel(std::string name, std::string creator, IRC& irc)
			: _name(name), _creator(creator), _irc(irc) {
	
	_users.push_back(_irc.get_user(creator));
	_operators.push_back(_irc.get_user(creator));
}

Channel::Channel(const Channel& x) : _name(x._name), _creator(x._creator), _irc(x._irc) {}

void Channel::add_user(User* user)
{
	_users.push_back(user);
}

bool Channel::is_banned(string nickname)
{
	int n = IRC::find_nickname(_banned, nickname);
	return n >= 0;
}