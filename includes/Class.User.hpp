#ifndef CLASS_USER_HPP
# define CLASS_USER_HPP

#include "./main.hpp"

# define CLIENT 0
# define USER	1

struct ModeUser
{
	bool i;
	bool s;
	bool w;
	bool o;
};

class User
{
private:
	std::string		nickname;
	std::string 	username;
	std::string 	hostname;
	std::string 	servername;
	std::string 	realname;
	int				hopcount;
	int				status;
	struct ModeUser	mode;

public:
	User(User const &src);
	User();
	User(std::string nickname, int hopcount = 0);
	User(std::string nickname, std::string username,
		 std::string hostname, std::string servername,
		 std::string realname, int hopcount = 0);
	User(std::string nickname, std::string username,
		 std::string hostname, std::string servername,
		 std::string realname, ModeUser mode, int hopcount = 0);

	void			client_from_pass(std::string nickname, int hopcount = 0);
	void			user_from_client(std::string username, std::string hostname, 
									 std::string servername, std::string realname);
	void			user_from_client(std::string username, std::string hostname,
									 std::string servername, std::string realname, ModeUser mode);

	int					getHopcount() const;
	std::string const &	getNickname() const;
};

//std::ostream &		operator<<(std::ostream &o, User const &src);

#endif