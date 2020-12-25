#include "main.hpp"
#include <set>
#include "Class.User.hpp"
#include "Class.User.hpp"

struct ModeChannel
{
	bool					private_mode;
	bool					secret_mode;
	bool					invite_only_mode;
	bool					moderated_mode;
	bool					limit_users_mode;
	bool					key_mode;
	bool					ban_mode;
};

class Channel
{
private:
	std::string				name;
	std::string				topic;
	//std::set<std::string>	users;
	//std::set<std::string>	operators;
	std::vector<User *>		users;
	std::vector<User *>		operators;
	std::vector<User *>		banned;
	ModeChannel				mode;
	int						limit_users;
	std::string				key;
	std::string				ban;

public:

};