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

	ModeChannel() { bzero(this, sizeof(*this)); }
};

class IRC;

class Channel
{
private:
	string			_name;
	string			_key;
	string			_creator;
	vector<User*>	_users;
	vector<User*>	_operators;
	vector<User*>	_banned;
	string			_topic;
	ModeChannel		_mode;
	// int				_limit_users;
	// string			_ban;
	IRC&			_irc;

public:
	Channel(const string& name, const string& key, const string& creator, IRC& irc);
	Channel(const Channel& x);
	Channel& operator=(const Channel& x);

	void 		add_user(User*);
	void 		add_operator(User*);
	void 		set_mode(const string&);
	bool 		is_banned(const string&) const;
	static bool is_valid_channel_name(const string&);

	const ModeChannel&	get_mode();
	const string&		get_key();
};