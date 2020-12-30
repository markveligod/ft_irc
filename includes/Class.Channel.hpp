#pragma once

#include <set>
#include "main.hpp"
#include "Class.IRC.hpp"
#include "Class.User.hpp"

using std::string;
using std::vector;
using std::set;

struct ModeChannel
{
	bool	private_mode;
	bool	secret_mode;
	bool	invite_only_mode;
	bool	moderated_mode;
	bool	limit_users_mode;
	bool	key_mode;
	bool	ban_mode;
};

class IRC;

class Channel
{
private:
	string			_name;
	string			_creator;
	vector<User *>	_users;
	vector<User *>	_operators;
	vector<User*>	_banned;
	string			_topic;
	ModeChannel		_mode;
	// int				_limit_users;
	// string			_key;
	// string			_ban;
	IRC&			_irc;

public:
	Channel(string name, string creator, IRC&);
	Channel(const Channel&);

	void add_user(User*);
	void add_operator(User*);
	void set_mode(string);
	bool is_banned(string);
};