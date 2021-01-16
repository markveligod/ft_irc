#pragma once

#include <set>
#include "main.hpp"
#include "Class.IRC.hpp"
#include "Class.User.hpp"

using std::string;
using std::vector;
using std::set;
using std::map;

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
	user_map		_users;
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

	const string&			getName() const;
	const string&			get_key() const;
	const string&			get_topic() const;
	user_map&	get_users();

	void					set_topic(const string& topic);
	void					set_operator(User* user);

	void 		add_user(User*);
	bool		is_local_channel() const;
	bool		is_network_channel() const;
	bool 		is_banned(User*) const;
	bool		is_valid_key(const string& key) const;
	bool		is_private() const;
	bool		is_secret() const;
	bool		is_invite_only() const;
	bool		is_moderated() const;
	bool		is_topic_only_oper() const;
	bool		is_user_in_channel(User*) const;
	bool		is_user_in_channel(int fd) const;
	bool		is_channel_operator(User* user) const;
	bool		is_have_voice(User* user) const;
	bool		is_visible() const;
	static bool is_valid_channel_name(const string&);


	void print_users() const
	{
		std::cout << std::setw(8) << "\t  Users: ";
		for (user_map::const_iterator it = _users.begin(); it != _users.end(); it++)
		{
			User u = *(it->first);
			if (it != _users.begin())
				std::cout << ", ";
			std::cout << u.getUsername();
		}
		std::cout << std::endl;
	}
};