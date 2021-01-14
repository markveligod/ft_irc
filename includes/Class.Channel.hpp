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
	bool	topic_only_oper_mode;

	ModeChannel() { bzero(this, sizeof(*this)); }
};

class IRC;

class Channel
{
private:
	string			_name;
	string			_key;
	User*			_creator;

	vector<User*>	_users;
	vector<User*>	_operators;
	vector<User*>	_users_have_voice;		// users, who not operators, but can speak on a moderated channel. MODE +v
	vector<User*>	_banned;

	string			_topic;
	ModeChannel		_mode;
	// int				_limit_users;
	// string			_ban;

public:
	Channel(const string& name = string());
	Channel(const string& name, const string& key, User* creator);
	Channel(const Channel& x);
	Channel& operator=(const Channel& x);

	void 		add_user(User*);
	void 		add_operator(User*);
	void 		set_mode(const string&);
	bool 		is_banned(const string&) const;
	bool		is_valid_key(const string& key) const;
	bool		is_private() const;
	bool		is_secret() const;
	bool		is_invite_only() const;
	bool		is_moderated() const;
	bool		is_topic_only_oper() const;
	bool		is_user_in_channel(const string&) const;
	bool		is_user_in_channel(int fd) const;
	bool		is_operator(const string&) const;
	bool		is_operator(int fd) const;
	bool		is_have_voice(const string&) const;
	bool		is_have_voice(int fd) const;
	bool		is_visible();
	static bool is_valid_channel_name(const string&);

	const string&		getName() const;
	const ModeChannel&	get_mode() const;
	const string&		get_key() const;
	const string&		get_topic() const;
	vector<User*>&		get_users();
	vector<User*>&		get_operators();

	void				set_topic(const string& topic);

	void print_users() const
	{
		std::cout << std::setw(8) << "\t  Users: ";
		for (size_t i = 0; i < _users.size(); i++)
		{
			User u = *(_users[i]);
			if (i != 0)
				std::cout << ", ";
			std::cout << u.getName();
		}
		std::cout << std::endl;
	}
};