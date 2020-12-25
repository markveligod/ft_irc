#include "./main.hpp"

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
	ModeUser		mode;

public:
	User();
	User(std::string nickname, std::string username,
		 std::string hostname, std::string servername,
		 std::string realname, int hopcount, ModeUser mode);
};