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
	bool	p;	// private_mode
	bool	s;	// secret_mode
	bool	i;	// invite_only_mode
	bool	t;	// topic_only_oper_mode
	bool	m;	// moderated_mode
	bool	l;	// limit_users_mode
	bool	b;	// ban_mode
	bool	k;	// key_mode

	bool	a; // a - переключить флаг анонимного канала;
	bool	n; // n - переключить отсутствие сообщений на канал от клиентов снаружи;
	bool	q; // q - переключить флаг тихого канала;
	bool	r; // r - переключить флаг канала перезапуска сервера;

	vector<string> ban_masks; //b - установить / удалить маску бана, чтобы пользователи не заходили;
	vector<string> exception_masks; //e - установить / удалить маску исключения для отмены маски бана;
	vector<string> invite_masks; //I - установить / удалить маску приглашения для автоматического переопределения флаг "только для приглашения";

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
	int				_limit_users;
	// string			_ban;

public:
	Channel(const string& name = string());
	Channel(const string& name, const string& key, User* creator);
	Channel(const Channel& x);
	Channel& operator=(const Channel& x);

	const string&			getName() const;
	const string&			get_key() const;
	const string&			get_topic() const;
	user_map&				get_users();
	ModeChannel*			getModeChannel();
	bool					getModeChannel(char mode);
	vector<User*>&			getBanned();

	void					set_topic(const string& topic);
	void					set_operator(User* user);
	void					del_operator(User* user);
	void					set_key(const string & new_key);
	void					set_limit_users(int new_limit);
	void					set_creator(User* creator);
	void					del_creator();
	void					set_voice(User* user);
	void					del_voice(User* user);

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