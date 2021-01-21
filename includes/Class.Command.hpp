
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
#include "Class.IRC.hpp"
#include "Class.User.hpp"
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
	int							nick_check_errors(int fd, int serv_client, IRC& irc);

	int							server_available(vector<Server *> &servers, string const &server_name) const;
	int							server_check_errors(IRC& irc, int fd) const;

	bool						pass(string password, string local_pass);

	void						user_create(Client* curr_client, vector<User*>& users, Server* curr_server);
	int							user_check_errors(IRC& irc, int fd);

	int							stats_check_errors(IRC &irc, int fd);
	void						stats_m(IRC& irc, int fd, Client* client);
	void						stats_l(IRC& irc, int fd, Client* client);
	void						stats_u(IRC& irc, int fd, Client* client);

	int							info_chek_errors(IRC &irc, int fd, int server_el, int client_el);
	int							version_check_errors(IRC &irc, int fd, int server_el, int client_el);
	int							links_check_errors(IRC &irc, int fd, int server_el, int client_el);
	vector<Server *>			find_by_mask(string const &str_mask, IRC &irc, bool &our_server);
	vector<Server *>			links_find(IRC &irc, int fd, string const & mask,  string const & our_servername, bool & our_server);

	void						join_channel(IRC &irc, const string &name, const string &key, User *user, int fd);
	void						leave_channel(IRC& irc, Channel& channels, int fd, string message);

	vector<Client*>::iterator	find_fd(vector<Client*>& vect, int fd);
	User*						find_user(user_map& users, int fd);
	bool						check_args_number(int n) const;
	bool						check_password(const Client& client) const;
	bool						check_nickname(const Client& client) const;
	void						send_channel_users(IRC& irc, int fd, User* user, Channel& channel);
	bool						is_channel_visible(IRC& irc, int fd, const string& channel_name);
	bool						is_server(IRC& irc, int fd);
	void						send_users_without_channel(IRC& irc, int fd);
	void						send_topic(IRC& irc, int fd, const string& channel_name, const string& topic);
	string						who_message(const User* user, string channel_name, string user_prefix);


public:
	Command(const string& str);
	~Command();

	int						cmd_nick(IRC& irc, int fd);
	int						cmd_pass(IRC& irc, int fd);
	int						cmd_user(IRC& irc, int fd);
	int						cmd_server(IRC& irc, int fd);
	int						cmd_join(IRC& irc, int fd);
	int						cmd_oper(IRC& irc, int fd);
	int						cmd_quit(IRC& irc, int fd);
	int						cmd_part(IRC& irc, int fd);
	int						cmd_names(IRC& irc, int fd);
	int						cmd_squit(IRC& irc, int fd);
	int						cmd_who(IRC& irc, int fd);
	int						cmd_topic(IRC& irc, int fd);
	int						cmd_ping(IRC& irc, int fd);
	int						cmd_pong(IRC& irc, int fd);
	int						cmd_mode(IRC& irc, int fd);
	int						cmd_kick(IRC& irc, int fd);
	int						cmd_privmsg(IRC& irc, int fd);
	int						cmd_away(IRC& irc, int fd);
	int						cmd_notice(IRC& irc, int fd);
	int						cmd_stats(IRC& irc, int fd);
	int						cmd_time(IRC& irc, int fd);
	int						cmd_error(IRC& irc, int fd);
	int						cmd_admin(IRC& irc, int fd);
	int						cmd_wallops(IRC& irc, int fd);
	int						cmd_connect(IRC& irc, int fd);
	int						cmd_info(IRC& irc, int fd);
	int						cmd_version(IRC& irc, int fd);
	int						cmd_links(IRC& irc, int fd);

	const string&			getCommand() const;
	const string&			getMessage() const;
	const string&			getPrefix() const;
	const vector<string>&	getArgs() const;
};