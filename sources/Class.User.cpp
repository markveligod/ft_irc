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
	_nickname		= src->getName();
	_hostname		= src->getHostname();
	_password		= src->getPassword();
	_hopcount		= src->getHopcount();
	_socket_fd		= src->getSocketFd();
	// client		= src;
	_channel_count	= 0;
}

User::
User(const User& src) { *this = src; }

User& User::
operator=(const User& src)
{
	_username = src._username;
	_hostname = src._hostname;
	_servername = src._servername;
	_realname = src._realname;
	// client = src.client;
	_mode = src._mode;
	_channel_count = src._channel_count;
	return *this;
}

/*User::User(const Client& src) : Client(src)
{
	_nickname	= src.getName();
	_password	= src.getPassword();
	_hopcount	= src.getHopcount();
	_socket_fd = src.getSocketFd();
	//client 	= src;
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
	_username		= username;
	_hostname		= hostname;
	_servername	= servername;
	_realname 		= realname;
	_mode.a		= 0;
	_mode.i		= 0;
	_mode.w	 	= 0;
	_mode.r		= 0;
	_mode.o		= 0;
	_mode.O		= 0;
	_channel_count	= 0;
}

void User::
user_from_client(const string& username, const string& hostname,
				 const string& servername, const string& realname,
				 const ModeUser& mode)
{
	_username		= username;
	_hostname		= hostname;
	_servername	= servername;
	_realname		= realname;
	_mode			= mode;
	_channel_count	= 0;
}

void User::
change_user(const string& username, const string& hostname,
			const string& servername, const string& realname)
{
	_username = username;
	_hostname = hostname;
	_servername = servername;
	_realname = realname;
}

void User::
setUsername(const string& username)		{ _username = username; }

void User::
setHostname(const string& hostname)		{ _hostname = hostname; }

void User::
setServername(const string& servername)	{ _servername = servername; }

void User::
setRealname(const string& realname)		{ _realname = realname; }

void User::
setMode(const ModeUser& mode)			{ _mode = mode; }

void User::
setMode(char mode, bool flag)
{
	switch (mode)
	{
		case 'a': _mode.a = flag; break;
		case 'i': _mode.i = flag; break;
		case 'w': _mode.w = flag; break;
		case 'r': _mode.r = flag; break;
		case 'o': _mode.o = flag; break;
		case 'O': _mode.O = flag; break;
		case 'v': _mode.v = flag; break;
		default: return;
	}
}

void User::
setAway(const string& mess)				{ _mode.away_message = mess; }

const string& User::
getUsername() const						{ return (_username); }

const string& User::
getNickname() const						{ return _nickname; }

const string& User::
getHostname() const						{ return (_hostname); }

const string& User::
getServername() const					{ return(_servername); }

const string& User::
getRealname() const						{ return (_realname); }

const ModeUser& User::
getMode() const							{ return _mode; }

bool User::
getMode(char m) const
{
	switch (m)
	{	
		case 'a': return _mode.a;
		case 'i': return _mode.i;
		case 'w': return _mode.w;
		case 'r': return _mode.r;
		case 'o': return _mode.o;
		case 'O': return _mode.O;
		case 'v': return _mode.v;
		default: return false;
	}
}

const string& User::
getAway() const							{ return _mode.away_message; }

size_t User::
getChannelCount() const					{ return (_channel_count); }

ModeUser& User::
getModeUser() 							{ return (_mode);}

bool User::
is_i_mode() const						{ return _mode.i; }

bool User::
is_o_mode() const						{ return _mode.o; }

void User::
inc_channel_count()						{ _channel_count++; }

void User::
dec_channel_count()						{ _channel_count--; }

void User::
print_user()
{
	std::cout << "socket "   << _socket_fd << std::endl;
	std::cout << "nick "     << _nickname << std::endl;
	std::cout << "username " << _username << std::endl;
}
