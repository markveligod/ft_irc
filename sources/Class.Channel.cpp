#include "Class.Channel.hpp"

Channel::
Channel(const string& name, const string& key, User* creator, IRC& irc)
			: _name(name), _key(key), _creator(creator), _irc(irc) {}

Channel::
Channel(const Channel& x): _irc(x._irc) {* this = x; }

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

void Channel::
add_operator(User* user)
{
	_operators.push_back(user);
}


bool Channel::
is_banned(const string& nickname) const
{
	int n = IRC::find_nickname(_banned, nickname);
	return n >= 0;
}

bool Channel::
is_valid_key(const string& key) const	{ return key == _key; }

bool Channel::
is_invite_only() const 					{ return _mode.invite_only_mode; }

bool Channel::
is_user_in_channel(const string& nickname) const
{
	int n = IRC::find_nickname(_users, nickname);
	return n >= 0;
}

bool Channel::
is_valid_channel_name(const string& name)
{
	if (name.size() > 200)
		return false;
	if (name.find(',') != string::npos
		|| name.find(' ') != string::npos
		|| name.find(7) != string::npos)
		return false;
	return true;
}

const string& Channel::
get_name() const		{ return _name; }

const ModeChannel& Channel::
get_mode() const		{ return _mode; }

const string& Channel::
get_key() const			{ return _key; }

vector<User*>& Channel::
get_users()				{ return _users; }

vector<User*>& Channel::
get_operators()			{ return _operators; }