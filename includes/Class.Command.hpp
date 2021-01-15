
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
#include "Class.User.hpp"
#include "Class.IRC.hpp"
#include "codes.hpp"

using std::string;
using std::vector;
using std::map;

class IRC;
class Server;
class Channel;

class Command
{
private:
	string			prefix;
	string			command;
	vector<string>	arguments;
	string			message;


	bool						nick_valid() const;
	template <typename T> bool	nick_available(vector<T> vect, const string& nick);
	int							server_available(vector<Server*> &servers, string const & server_name) const;
	int							server_check_errors(vector<Server *> &servers,vector<Client *> &clients, int fd) const;
	bool						pass(string password, string local_pass);
	void						user_change(User* curr_user);
	void						user_create(Client* curr_client, vector<User*>& users, Server* curr_server);
	vector<Client*>::iterator	find_fd(vector<Client*>& vect, int fd);
	bool						check_args_number(int n) const;
	bool						check_password(const Client& client) const;
	bool						check_nickname(const Client& client) const;
	void						leave_channel(IRC& irc, Channel& channels, char type, int fd, string message);
	void						send_channel_users(IRC& irc, int fd, char type, string cnannel_name);
	bool						is_channel_visible(IRC& irc, int fd, char channel_type, const string& channel_name);
	bool						is_server(IRC& irc, int fd);
	void						send_users_without_channel(IRC& irc, int fd);
	void						join_channel(IRC& irc, const string& name, const string& key, char type, const string& nickname, int fd);
	int							join_from_server(IRC& irc, const string& channel_name, const string& nickname);
	void						send_topic(IRC& irc, int fd, const string& channel_name, const string& topic);
	string						who_message(const User& user);


public:
	Command(const string& str);
	~Command();

	int					cmd_nick(IRC& irc, int fd);
	int					cmd_pass(IRC& irc, int fd);
	int					cmd_user(IRC& irc, int fd);
	int					cmd_server(IRC& irc, int fd);
	int					cmd_join(IRC& irc, int fd);
	int					cmd_oper(IRC& irc, int fd);
	int					cmd_quit(IRC& irc, int fd);
	int					cmd_part(IRC& irc, int fd);
	int					cmd_names(IRC& irc, int fd);
	int					cmd_squit(IRC& irc, int fd);
	int					cmd_who(IRC& irc, int fd);
	int					cmd_topic(IRC& irc, int fd);
	int					cmd_ping(IRC& irc, int fd);
	int					cmd_pong(IRC& irc, int fd);

	const string&		getCommand() const;
	const string&		getMessage() const;
};