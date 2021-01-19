#pragma once

#include "main.hpp"
#include "Class.Statistics.hpp"

using std::string;

class Client
{
protected:
	string					nickname;
	string					hostname;
	int						socket_fd;
	int						hopcount;
	bool					password;
	string					buffer;
	bool					is_server;
	Statistics				statistics;

public:
	Client(string const &hostname = "", int socket_fd = 0, int hopcount = 0);
	Client(int socket_fd, const string& nickname, const string& hostname = "", int hopcount = 0);
	Client(const Client& src);
	Client& operator=(const Client&);

	void					setNickname(const string& nickname);
	void					setHopcount(int hopcount);
	void					setPassword(bool password = true);
	void					setBuffer(const string& buff);
	void					setIsServer(bool state);

	int						getHopcount() const;
	const string&			getName() const;
	bool					getPassword() const;
	int						getSocketFd() const;
	const string&			getBuffer() const;
	bool					getIsServer() const;
	const string&			getHostname() const;
	Statistics &			getStatistics();

	bool					find_line_break();
	string					get_line_break();
	bool					isempty_buffer();
};
