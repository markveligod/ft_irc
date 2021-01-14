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
	return *this;
}

const string& Channel::
getName() const										{ return _name; }

const ModeChannel& Channel::
get_mode() const									{ return _mode; }

const string& Channel::
get_key() const										{ return _key; }

const string& Channel::
get_topic() const									{ return _topic; }

vector<User*>& Channel::
get_users()											{ return _users; }

vector<User*>& Channel::
get_operators()										{ return _operators; }

void Channel::
set_topic(const string& topic)						{ _topic = topic; }

void Channel::
add_user(User* user)								{ _users.push_back(user); }

void Channel::
add_operator(User* user)							{ _operators.push_back(user); }

bool Channel::
is_banned(const string& nickname) const				{ return IRC::find_name(_banned, nickname) >= 0; }

bool Channel::
is_valid_key(const string& key) const				{ return key == _key; }

bool Channel::
is_private() const	 								{ return _mode.private_mode; }

bool Channel::
is_secret() const 									{ return _mode.secret_mode; }

bool Channel::
is_invite_only() const 								{ return _mode.invite_only_mode; }

bool Channel::
is_moderated() const 								{ return _mode.moderated_mode; }

bool Channel::
is_topic_only_oper() const 							{ return _mode.topic_only_oper_mode; }

bool Channel::
is_user_in_channel(const string& nickname) const	{ return IRC::find_name(_users, nickname) >= 0; }

bool Channel::
is_user_in_channel(int fd) const					{ return IRC::find_fd(_users, fd) >= 0; }

bool Channel::
is_operator(const string& nickname) const			{ return IRC::find_name(_operators, nickname) >= 0; }

bool Channel::
is_operator(int fd) const							{ return IRC::find_fd(_operators, fd) >= 0; }

bool Channel::
is_have_voice(const string& nickname) const			{ return IRC::find_name(_users_have_voice, nickname) >= 0; }

bool Channel::
is_have_voice(int fd) const							{ return IRC::find_fd(_users_have_voice, fd) >= 0; }

bool Channel::
is_visible()										{ return !is_private() && !is_secret(); }

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
