#include "Class.Channel.hpp"

Channel::Channel(const string& name, const string& key, const string& creator, IRC& irc)
			: _name(name), _key(key), _creator(creator), _irc(irc) {
	
	_users.push_back(_irc.get_user(creator));
	_operators.push_back(_irc.get_user(creator));
}

Channel::Channel(const Channel& x)
			: _name(x._name), _key(x._key), _creator(x._creator), _irc(x._irc) {}

void Channel::add_user(User* user)
{
	_users.push_back(user);
}

bool Channel::is_banned(const string& nickname) const
{
	int n = IRC::find_nickname(_banned, nickname);
	return n >= 0;
}

const ModeChannel&	Channel::get_mode() { return _mode; }

const string& 		Channel::get_key() { return _key; }