#include "Class.User.hpp"
#include "Class.Client.hpp"

/*
** ----------------------------------------------------------
** Constructor
** ----------------------------------------------------------
*/

User::User(Client * src) : Client(*src)
{
	this->nickname	= src->getNickname();
	this->password	= src->getPassword();
	this->hopcount	= src->getHopcount();
	this->socket_fd = src->getSocketFd();
	this->client 	= src;
}

/*User::User(Client const &src) : Client(src)
{
	this->nickname	= src.getNickname();
	this->password	= src.getPassword();
	this->hopcount	= src.getHopcount();
	this->socket_fd = src.getSocketFd();
	//this->client 	= src;
}*/

/*
** ---------------------------------------------------------------
** creating user from client
**
** когда клиент уже ввел пароль и задал nick, и командой USER
** хочет создать юзера, вызываем эту функцию
** 
** она перегружена - первый вариант без задания модов юзера
**					 (например, когда создается новый юзер)
**				   - второй вариант с заданием модов
**					 (например, когда юзеры приходят от сервера)
** ---------------------------------------------------------------
*/

void User::user_from_client(std::string username, std::string hostname,
					  std::string servername, std::string realname)
{
	this->username	 = username;
	this->hostname	 = hostname;
	this->servername = servername;
	this->realname 	 = realname;
	this->mode.i	 = 0;
	this->mode.o	 = 0;
	this->mode.s	 = 0;
	this->mode.w	 = 0;
}

void User::user_from_client(std::string username, std::string hostname,
					  std::string servername, std::string realname, ModeUser mode)
{
	this->username 	 = username;
	this->hostname	 = hostname;
	this->servername = servername;
	this->realname	 = realname;
	this->mode		 = mode;
}

void	User::change_user(std::string const & username, std::string const & hostname,
						  std::string const & servername, std::string const realname)
{
	this->username = username;
	this->hostname = hostname;
	this->servername = servername;
	this->realname = realname;
}

void User::setUsername(std::string const &username) { this->username = username; }
void User::setHostname(std::string const &hostname) { this->hostname = hostname; }
void User::setServername(std::string const &servername) { this->servername = servername; }
void User::setRealname(std::string const &realname) { this->realname = realname; }
void User::setMode(ModeUser mode) { this->mode = mode; }

void User::print_user()
{
	std::cout << "socket " << this->socket_fd << std::endl;
	std::cout << "nick " << this->nickname << std::endl;
	std::cout << "username " << this->username << std::endl;
}