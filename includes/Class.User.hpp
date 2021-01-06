#ifndef CLASS_USER_HPP
# define CLASS_USER_HPP

#include "main.hpp"
#include "Class.Client.hpp"

using std::string;

struct ModeUser
{
	bool i;
	bool s;
	bool w;
	bool o;

	ModeUser& operator=(const ModeUser&);
};

class User : public Client
{
private:
	string 		username;
	string 		hostname;
	string 		servername;
	string 		realname;
	Client*		client;
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
	void			print_user();

	const string&	getUsername() const;
	const string&	getNickname() const;
	const string&	getHostname() const;
	const string&	getServername() const;
	const string&	getRealname() const;

	void			inc_channel_count();
	void			dec_channel_count();
};

#endif