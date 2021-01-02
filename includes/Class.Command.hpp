
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"
class IRC;
class Server;
#include "Class.User.hpp"
#include "Class.IRC.hpp"
#include "codes.hpp"

using std::string;
using std::vector;

class Command
{
private:
	string				prefix;
	string				command;
	vector<string> 		arguments;


	bool							nick_valid() const;
	template <typename T> bool		nick_available(vector<T> vect, const string& nick);

	bool							pass(string password, string local_pass);
	void							user_change(User* curr_user);
	void							user_create(Client* curr_client, vector<User*>& users, Server* curr_server);
	vector<Client *>::iterator		find_fd(vector<Client*>& vect, int fd);

public:
	Command(string const & str);
	~Command();

	int					cmd_nick(IRC& irc, int fd);
	int					cmd_pass(IRC& irc, int fd);
	int					cmd_user(IRC& irc, int fd);
	int					cmd_server(IRC& irc, int fd);
	int					cmd_join(IRC& irc, int fd);

	const string&		getCommand() const;
	bool				check_args_number(int n) const;
	bool				check_password(Client const & client) const;
	bool				check_nickname(Client const & client) const;
};