#include "Class.Channel.hpp"

Channel::
Channel(const string& name, const string& key, const string& creator, IRC& irc)
			: _name(name), _key(key), _creator(creator), _irc(irc) {
	
	_users.push_back(_irc.get_user(creator));
	_operators.push_back(_irc.get_user(creator));
}

Channel::
Channel(const Channel& x): _irc(x._irc) { *this = x; }

Channel& Channel::operator=(const Channel& x)
{
	_name = x._name;
	_key = x._key;
	_creator = x._creator;
	return *this;
}

void Channel::
add_user(User* user)
{
	_users.push_back(user);
}

bool Channel::
is_banned(const string& nickname) const
{
	int n = IRC::find_nickname(_banned, nickname);
	return n >= 0;
}

bool Channel::
is_valid_channel_name(const string& name)
{
	if (name.size() > 200)
		return false;
	if (name.find(',') != string::npos || name.find(' ') != string::npos)
		return false;
	return true;
}

const ModeChannel& Channel::
get_mode() { return _mode; }

const string& Channel::
get_key() { return _key; }