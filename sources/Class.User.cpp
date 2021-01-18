#include "Class.User.hpp"
#include "Class.Client.hpp"

using std::string;

/*
** ----------------------------------------------------------
** Constructor
** ----------------------------------------------------------
*/

ModeUser::
ModeUser() : a(false),
			 i(false),
			 w(false),
			 r(false),
			 o(false),
			 O(false),
			 v(false),
			 away_message(string()) {}

ModeUser::
ModeUser(const ModeUser& src) { *this = src; }

ModeUser& ModeUser::
operator=(const ModeUser& src)
{
	a = src.a;
	i = src.i;
	w = src.w;
	r = src.r;
	o = src.o;
	O = src.O;
	v = src.v;
	away_message = string();

	return* this;
}

User::
User(Client* src) : Client(*src)
{
	this->nickname		= src->getName();
	this->hostname		= src->getHostname();
	this->password		= src->getPassword();
	this->hopcount		= src->getHopcount();
	this->socket_fd		= src->getSocketFd();
	// this->client		= src;
	this->channel_count	= 0;
}

User::
User(const User& src) { *this = src; }

User& User::
operator=(const User& src)
{
	username = src.username;
	hostname = src.hostname;
	servername = src.servername;
	realname = src.realname;
	// client = src.client;
	mode = src.mode;
	channel_count = src.channel_count;
	return *this;
}

/*User::User(const Client& src) : Client(src)
{
	this->nickname	= src.getName();
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

void User::
user_from_client(const string& username, const string& hostname, 
				 const string& servername, const string& realname)
{
	this->username		= username;
	this->hostname		= hostname;
	this->servername	= servername;
	this->realname 		= realname;
	this->mode.a		= 0;
	this->mode.i		= 0;
	this->mode.w	 	= 0;
	this->mode.r		= 0;
	this->mode.o		= 0;
	this->mode.O		= 0;
	this->channel_count	= 0;
}

void User::
user_from_client(const string& username, const string& hostname,
				 const string& servername, const string& realname,
				 const ModeUser& mode)
{
	this->username		= username;
	this->hostname		= hostname;
	this->servername	= servername;
	this->realname		= realname;
	this->mode			= mode;
	this->channel_count	= 0;
}

void User::
change_user(const string& username, const string& hostname,
			const string& servername, const string& realname)
{
	this->username = username;
	this->hostname = hostname;
	this->servername = servername;
	this->realname = realname;
}

void User::
setUsername(const string& username)		{ this->username = username; }

void User::
setHostname(const string& hostname)		{ this->hostname = hostname; }

void User::
setServername(const string& servername)	{ this->servername = servername; }

void User::
setRealname(const string& realname)		{ this->realname = realname; }

void User::
setMode(const ModeUser& mode)			{ this->mode = mode; }

void User::
setMode(char mode, bool flag)
{
	switch (mode)
	{
		case 'a': this->mode.a = flag; break;
		case 'i': this->mode.i = flag; break;
		case 'w': this->mode.w = flag; break;
		case 'r': this->mode.r = flag; break;
		case 'o': this->mode.o = flag; break;
		case 'O': this->mode.O = flag; break;
		case 'v': this->mode.v = flag; break;
		default: return;
	}
}

void User::
setAway(const string& mess)				{ mode.away_message = mess; }

const string& User::
getUsername() const						{ return (this->username); }

const string& User::
getNickname() const						{ return this->nickname; }

const string& User::
getHostname() const						{ return (this->hostname); }

const string& User::
getServername() const					{ return(this->servername); }

const string& User::
getRealname() const						{ return (this->realname); }

const ModeUser& User::
getMode() const							{ return mode; }

bool User::
getMode(char m) const
{
	switch (m)
	{	
		case 'a': return mode.a;
		case 'i': return mode.i;
		case 'w': return mode.w;
		case 'r': return mode.r;
		case 'o': return mode.o;
		case 'O': return mode.O;
		case 'v': return mode.v;
		default: return false;
	}
}

const string& User::
getAway() const							{ return mode.away_message; }

size_t User::
getChannelCount() const					{ return (this->channel_count); }

ModeUser& User::
getModeUser() 							{ return (this->mode);}

// int User::
// getFd() const							{ return (this->client->getSocketFd()); }

// int User::
// getHopcount() const						{ return (this->client->getHopcount()); }

bool User::
is_i_mode() const						{ return mode.i; }

bool User::
is_o_mode() const						{ return mode.o; }

void User::
inc_channel_count()						{ channel_count++; }

void User::
dec_channel_count()						{ channel_count--; }

void User::
print_user()
{
	std::cout << "socket "   << this->socket_fd << std::endl;
	std::cout << "nick "     << this->nickname << std::endl;
	std::cout << "username " << this->username << std::endl;
}
