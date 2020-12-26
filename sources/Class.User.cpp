#include "Class.User.hpp"

// -------------
// Constructors
// -------------

User::User() {}

User::User(User const & src)
		  : nickname(src.nickname), username(src.username),
		    hostname(src.hostname), servername(src.servername),
			realname(src.realname), hopcount(src.hopcount), 
			status(src.status), mode(src.mode) {}

User::User(std::string nickname, int hopcount) :
			nickname(nickname), hopcount(hopcount)
{
	this->status = CLIENT;
}

User::User(std::string nickname, std::string username,
		   std::string hostname, std::string servername,
		   std::string realname, int hopcount)
	: nickname(nickname), username(username), hostname(hostname),
	  servername(servername), realname(realname), hopcount(hopcount)
{
	this->status = USER;
	this->mode.i = 0;
	this->mode.o = 0;
	this->mode.s = 0;
	this->mode.w = 0;
}

User::User(std::string nickname, std::string username,
		   std::string hostname, std::string servername,
		   std::string realname, ModeUser mode, int hopcount)
	: nickname(nickname), username(username), hostname(hostname),
	  servername(servername), realname(realname), hopcount(hopcount),
	  mode(mode)
{
	this->status = USER;
}

// ----------------------------------------------
// Creating user from client (with USER command)
// ----------------------------------------------

void User::client_from_pass(std::string nickname, int hopcount)
{
	this->nickname = nickname;
	this->hopcount = hopcount;
}

void User::user_from_client(std::string username, std::string hostname,
					  std::string servername, std::string realname)
{
	this->username = username;
	this->hostname = hostname;
	this->servername = servername;
	this->realname = realname;
	this->mode.i = 0;
	this->mode.o = 0;
	this->mode.s = 0;
	this->mode.w = 0;
}

void User::user_from_client(std::string username, std::string hostname,
					  std::string servername, std::string realname, ModeUser mode)
{
	this->username = username;
	this->hostname = hostname;
	this->servername = servername;
	this->realname = realname;
	this->mode = mode;
}

int User::getHopcount() const {return (this->hopcount);}
std::string const & User::getNickname() const {return (this->nickname);}

/*std::ostream &operator<<(std::ostream &o, User const &src)
{
	o << src.getName() << ", bureaucrat grade " << src.getGrade();
	return (o);
}*/
