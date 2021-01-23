
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
	string			_prefix;
	string			_command;
	vector<string>	_arguments;
	string			_message;


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

	int 						trace_check_errors(IRC &irc, int fd, int client_el);
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
	void						send_whois(IRC& irc, int fd, User* recepient, User* sender);
	string						who_message(const User* user, string channel_name, string user_prefix);


public:
	Command(const string& str);
	~Command();

	void					cmd_nick(IRC& irc, int fd);
	void					cmd_pass(IRC& irc, int fd);
	void					cmd_user(IRC& irc, int fd);
	void					cmd_server(IRC& irc, int fd);
	void					cmd_join(IRC& irc, int fd);
	void					cmd_oper(IRC& irc, int fd);
	void					cmd_quit(IRC& irc, int fd);
	void					cmd_part(IRC& irc, int fd);
	void					cmd_names(IRC& irc, int fd);
	void					cmd_squit(IRC& irc, int fd);
	void					cmd_who(IRC& irc, int fd);
	void					cmd_topic(IRC& irc, int fd);
	void					cmd_ping(IRC& irc, int fd);
	void					cmd_pong(IRC& irc, int fd);
	void					cmd_mode(IRC& irc, int fd);
	void					cmd_kick(IRC& irc, int fd);
	void					cmd_privmsg(IRC& irc, int fd);
	void					cmd_away(IRC& irc, int fd);
	void					cmd_notice(IRC& irc, int fd);
	void					cmd_stats(IRC& irc, int fd);
	void					cmd_time(IRC& irc, int fd);
	void					cmd_error(IRC& irc, int fd);
	void					cmd_admin(IRC& irc, int fd);
	void					cmd_wallops(IRC& irc, int fd);
	void					cmd_connect(IRC& irc, int fd);
	void					cmd_info(IRC& irc, int fd);
	void					cmd_version(IRC& irc, int fd);
	void					cmd_links(IRC& irc, int fd);
	void					cmd_trace(IRC& irc, int fd);
	void					cmd_motd(IRC& irc, int fd);
	void					cmd_kill(IRC& irc, int fd);
	void					cmd_whois(IRC& irc, int fd);
	void					cmd_list(IRC& irc, int fd);
	void					cmd_lusers(IRC& irc, int fd);

	const string&			getCommand() const;
	const string&			getMessage() const;
	const string&			getPrefix() const;
	const vector<string>&	getArgs() const;
};