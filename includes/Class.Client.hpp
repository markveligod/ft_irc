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
	std::string			username;
	std::string			hostname;
	std::string			servername;
	std::string			realname;
	bool				status_ready;

public:
	Client(int socket_fd);
	Client(Client const &src);

	void				setNickname(std::string const & nickname);
	void				setHopcount(int hopcount);
	void				setPassword();
	void				setUsername(std::string const &username);
	void				setHostname(std::string const &hostname);
	void				setServername(std::string const &servername);
	void				setRealname(std::string const &realname);
	void				setStatusReady(bool status);

	int					getHopcount() const;
	std::string const &	getNickname() const;
	bool				getPassword() const;
	int					getSocketFd() const;
	std::string const &	getUsername() const;
	std::string const &	getHostname() const;
	std::string const &	getServername() const;
	std::string const &	getRealname() const;
	bool				getStatusReady() const;
};

#endif