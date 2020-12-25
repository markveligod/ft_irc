

struct User {
	int				fd;
	std::string		username;
	std::string		hostname;
	std::string		servername;
	std::string		realname;
	int				distance;
	bool			banned;
	ModeUser		mode;
}

struct Channel {
	std::string				name;
	std::set<std::string>	users;
	std::set<std::string>	operators;
	ModeChanel				mode;
}

struct ModeUser {

}

struct ModeChanel {

}

class Command {
	enum {PASS, NICK};
	pass(std::string) {if (true); }
	nick(std::string) {add user; }
	(*void)()[] = {pass, nick};
	command(int);
}