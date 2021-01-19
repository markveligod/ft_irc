#pragma once

#include "main.hpp"
#include "Class.Client.hpp"

class User;
struct ModeUser;
typedef std::map<User*, ModeUser>			user_map;
typedef std::map<User*, ModeUser>::iterator	user_iterator;

using std::string;

struct ModeUser
{
	// https://tools.ietf.org/html/rfc2812#section-3.1.5

	bool a;
	bool i;
	bool w;
	bool r;
	bool o;
	bool O;
	bool v;
	string away_message;

	ModeUser();
	ModeUser(const ModeUser&);
	ModeUser& operator=(const ModeUser&);
};

class User : public Client
{
private:
	string 		username;
	string 		hostname;
	string 		servername;
	string 		realname;
	// Client*		client;
	ModeUser	mode;
	size_t		channel_count;

public:
	User(Client* src);
	User(const Client& src);
	User(const User&);
	User& operator=(const User&);

	void			user_from_client(const string& username, const string& hostname, 
									 const string& servername, const string& realname);
	void			user_from_client(const string& username, const string& hostname,
									 const string& servername, const string& realname, const ModeUser& mode);

	void			change_user(const string& username, const string& hostname,
								const string& servername, const string& realname);

	void			setUsername(const string& username);
	void			setHostname(const string& hostname);
	void			setServername(const string& servername);
	void			setRealname(const string& realname);
	void			setMode(const ModeUser& mode);
	void			setMode(char mode, bool flag);
	void			setAway(const string& away_message);
	void			print_user();

	const string&	getUsername() const;
	const string&	getNickname() const;
	const string&	getHostname() const;
	const string&	getServername() const;
	const string&	getRealname() const;
	const ModeUser&	getMode() const;
	bool			getMode(char mode) const;
	const string&	getAway() const;
	size_t			getChannelCount() const;
	// int				getFd() const;
	// int				getHopcount() const;
	ModeUser&		getModeUser();
	bool			is_i_mode() const;
	bool			is_o_mode() const;

	void			inc_channel_count();
	void			dec_channel_count();
};
