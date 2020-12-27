#ifndef CLASS_CLIENT_HPP
# define CLASS_CLIENT_HPP

#include "main.hpp"

class Client
{
protected:
	std::string			nickname;
	int					socket_fd;
	int					hopcount;
	bool				password;

public:
	Client(int socket_fd);
	Client(Client const &src);

	void				setNickname(std::string const & nickname);
	void				setHopcount(int hopcount);
	void				setPassword();

	int					getHopcount() const;
	std::string const &	getNickname() const;
	bool				getPassword() const;
	int					getSocketFd() const;
};

#endif