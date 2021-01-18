#include "Class.Channel.hpp"

Channel::
Channel(const string& name)
			: _name(name), _key(string()) {}

Channel::
Channel(const string& name, const string& key, User* creator)
			: _name(name), _key(key), _creator(creator) {}

Channel::
Channel(const Channel& x) { *this = x; }

Channel& Channel::operator=(const Channel& x)
{
	_name = x._name;
	_key = x._key;
	_creator = x._creator;
	_users = x._users;
	_topic = x._topic;
	_mode = x._mode;
	return *this;
}

const string& Channel::
getName() const										{ return _name; }

const string& Channel::
get_key() const										{ return _key; }

const string& Channel::
get_topic() const									{ return _topic; }

user_map& Channel::
get_users()											{ return _users; }

ModeChannel* Channel::
getModeChannel() 									{ return (&(this->_mode));}

vector<User*>& Channel::
getVecBanned()										{ return (this->_banned);}

bool Channel::
getModeChannel(char mode)
{
	switch (mode)
	{
		case 'p': return _mode.p;
		case 's': return _mode.s;
		case 'i': return _mode.i;
		case 't': return _mode.t;
		case 'm': return _mode.m;
		case 'l': return _mode.l;
		case 'b': return _mode.b;
		case 'k': return _mode.k;
		case 'a': return _mode.a;
		case 'n': return _mode.n;
		case 'q': return _mode.q;
		case 'r': return _mode.r;
		default: return false;
	}
}

void Channel::
set_topic(const string& topic)						{ _topic = topic; }

void Channel::
set_key(const string & new_key)						{ this->_key = new_key;}

void Channel::
set_limit_users(int new_limit)						{ this->_limit_users = new_limit;}

void Channel::
add_user(User* user)
{
	if (!_users.count(user)) 
		_users[user] = ModeUser();
}

bool Channel::
is_local_channel() const							{ return _name[0] == '&'; }

bool Channel::
is_network_channel() const							{ return _name[0] == '#'; }

void Channel::
set_operator(User* user)							{ _users[user].o = true; }

void Channel::
del_operator(User* user)							{ _users[user].o = false; }

void Channel::
set_creator(User* creator)							{ this->_creator = creator;}

void Channel::
del_creator()										{ this->_creator = NULL;}

void Channel::
set_voice(User* user) 								{ _users[user].v = true;}

void Channel::
del_voice(User* user)								{ _users[user].v = false;}

bool Channel::
is_banned(User* user) const							
{
	return find(_banned.begin(), _banned.end(), user) != _banned.end();
}

bool Channel::
is_valid_key(const string& key) const				{ return is_equal(key, _key); }

bool Channel::
is_private() const	 								{ return _mode.p; }

bool Channel::
is_secret() const 									{ return _mode.s; }

bool Channel::
is_invite_only() const 								{ return _mode.i; }

bool Channel::
is_moderated() const 								{ return _mode.m; }

bool Channel::
is_topic_only_oper() const 							{ return _mode.t; }

bool Channel::
is_user_in_channel(User* user) const				{ return _users.count(user); }

bool Channel::
is_channel_operator(User* user) const				{ return _users.find(user)->second.o; }

bool Channel::
is_have_voice(User* user) const						{ return _users.find(user)->second.v; }

bool Channel::
is_visible() const									{ return !is_private() && !is_secret(); }

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
