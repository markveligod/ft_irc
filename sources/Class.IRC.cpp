#include "Class.IRC.hpp"

# define FD_FREE 0
# define FD_CLIENT 1
# define FD_SERVER 2
# define FD_CLIENT_SSL 3
# define FD_SERVER_SSL 4

# define CERTIFICATE "cert/cert.pem"
# define PRIVATE_KEY "cert/key.pem"
std::string g_cmd_name[COMM_COUNT] = {"NICK",
									  "PASS",
									  "USER",
									  "SERVER",
									  "JOIN",
									  "OPER",
									  "QUIT",
									  "PART",
									  "NAMES",
									  "SQUIT",
									  "WHO",
									  "TOPIC",
									  "PING",
									  "PONG",
									  "MODE",
									  "KICK",
									  "PRIVMSG",
									  "AWAY",
									  "NOTICE",
									  "STATS",
									  "TIME",
									  "ERROR",
									  "ADMIN",
									  "WALLOPS",
									  "CONNECT",
									  "INFO",
									  "VERSION",
									  "LINKS",
									  "TRACE",
									  "KILL",
									  "MOTD",
									  "WHOIS",
									  "LIST",
									  "LUSERS"
									  };
/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

CmdStats::
	CmdStats()
{
}

CmdStats::
CmdStats(string cmd_name) : cmd_name(cmd_name),
							count(0),
							byte_count(0) {}

IRC::
IRC() {}

IRC::
IRC(string network_ip,
	string network_port,
	string network_pass,
	string localhost_port,
	string localhost_pass,
	string operator_user,
	string operator_pass)
{
	_server_name = SERVERNAME + string("/") + localhost_port;
	_host_name = LOCALHOST;
	_operator_user = operator_user;
	_operator_pass = operator_pass;
	if (!network_ip.empty())
	{
		_network_ip.push_back(network_ip);
		_network_port.push_back(std::atoi(network_port.c_str()));
		_network_pass.push_back(network_pass);
	}
	_localhost_pass = localhost_pass;
	int port = std::atoi(localhost_port.c_str());
	_localhost = Socket(LOCALHOST, port);
	_localhost_ssl = Socket(LOCALHOST, port + 1);
	utils::print_line("Constructor IRC done!");
	utils::print_line("Socket local done!");
	//generate_map_cmd_stats();
}

IRC::
~IRC()
{
	SSL_CTX_free(_ctx);
}

/*
** ----------------------------------------------------------
** Creating sockets
** ----------------------------------------------------------
*/

void IRC::
create_socket_local()
{
	// local socket created on IRC conctructor

	_array_fd_select[_localhost._socket()] = FD_SERVER;
	utils::print_line("Socket local FD done!");

	_array_fd_select[_localhost_ssl._socket()] = FD_SERVER_SSL;
	utils::print_line("Socket ssl local FD done!");

	_localhost._bind();
	utils::print_line("Socket local bind done!");

	_localhost_ssl._bind();
	utils::print_line("Socket ssl local bind done!");

	_localhost._listen();
	utils::print_line("Socket local listen...");

	_localhost_ssl._listen();
	utils::print_line("Socket ssl local listen...");
}

void IRC::
create_socket_network()				// network_ip, _network_port и network_pass добавили заранееe
{
	size_t n = _network_ip.size() - 1;

	_network.push_back(Socket(_network_ip[n].c_str(), _network_port[n]));
	utils::print_line("Socket network done!");

	int fd = _network[n]._socket();
	utils::print_line("Socket network FD done!");

	if ((_network[n]._connect()) < 0)
		return;
	utils::print_line("Socket network connection!");

	_array_fd_select[fd] = FD_CLIENT;
	_clients.push_back(new Client(_network_ip[n], fd));
	
	utils::print_line("Trying to connect to server!\nServer name: " +
					  _network_ip[n] + "/" + utils::int_to_str(_network_port[n]) +
					  "\nHopcount: 1\nInfo: info");

	push_cmd_queue(fd, "PASS " + _network_pass[n] + "\r\nSERVER " + _server_name + " 1 :" + INFO + "\r\n");
	_clients[_clients.size() - 1]->setIsServer(true);
}

/*
** ---------------------------------------------------------------------------------------------
** do_command 	- выполняет команду, которая пришла в сообщении от клиента
** 				  (принимает на вход команду и дескриптор сокета, от которого она пришла)
** суть работы -  создает массив имен команд, ссылок на соответствующие функции,
**				  и два массива аргументов для этих функций;
**				  Проходится в цикле по всем именам команд и запускает соответствующую функцию
**				  (метод класса message)
** ---------------------------------------------------------------------------------------------
*/

typedef void (Command::*doCommand)(IRC& irc, int fd);
typedef void (IRC::*SignalHandlerPointer)(int);

void IRC::
do_command(Command* command, int fd)
{
	doCommand	cmd_func[COMM_COUNT] = {&Command::cmd_nick,
										&Command::cmd_pass,
										&Command::cmd_user,
										&Command::cmd_server,
										&Command::cmd_join,
										&Command::cmd_oper,
										&Command::cmd_quit,
										&Command::cmd_part,
										&Command::cmd_names,
										&Command::cmd_squit,
										&Command::cmd_who,
										&Command::cmd_topic,
										&Command::cmd_ping,
										&Command::cmd_pong,
										&Command::cmd_mode,
										&Command::cmd_kick,
										&Command::cmd_privmsg,
										&Command::cmd_away,
										&Command::cmd_notice,
										&Command::cmd_stats,
										&Command::cmd_time,
										&Command::cmd_error,
										&Command::cmd_admin,
										&Command::cmd_wallops,
										&Command::cmd_connect,
										&Command::cmd_info,
										&Command::cmd_version,
										&Command::cmd_links,
										&Command::cmd_trace,
										&Command::cmd_kill,
										&Command::cmd_motd,
										&Command::cmd_whois,
										&Command::cmd_list,
										&Command::cmd_lusers
										};

	const string & comm 			= command->getCommand();
	int client_el					= IRC::find_fd(_clients, fd);
	int server_el					= IRC::find_fd(_servers, fd);

	_statistics.recieved(comm, command->getMessage());
	if (server_el >= 0)
		_servers[server_el]->getStatistics().recieved(comm, command->getMessage());
	else if (client_el >= 0)
		_clients[client_el]->getStatistics().recieved(comm, command->getMessage());

	if (comm == "")
	{
		push_cmd_queue(fd, "ERROR :Prefix without command\r\n");
		return;
	}

	if (!(is_equal(comm, "PASS") || is_equal(comm, "451")
		|| (client_el >= 0 && _clients[client_el]->getPassword()
			&& (is_equal(comm, "NICK") || is_equal(comm, "USER") || is_equal(comm, "SERVER")))	// client entered correct pass
		|| IRC::find_fd(_servers, fd) >= 0								// client is registred as Server
		|| IRC::find_fd(_users, fd) >= 0))								// client is registred as User
	{
		string message = ":" + _server_name + " " +
						 utils::int_to_str(ERR_NOTREGISTERED) + " "
						 "*" + ERR_NOTREGISTERED_MESS + "\r\n";
		push_cmd_queue(fd, message);
		return;
	}

	for (int i = 0; i < COMM_COUNT; i++)
	{
		if (is_equal(g_cmd_name[i], comm))
		{
			utils::print_command(fd, command->getMessage());
			(command->*(cmd_func[i]))(*this, fd);
			return;
		}
	}
	if(is_numeric_response(*command))
		return;
	
	push_cmd_queue(fd, response(ERR_UNKNOWNCOMMAND, fd, comm, ERR_UNKNOWNCOMMAND_MESS));
}

/*
** ======================SELECT PART=========================
**
** ----------------------------------------------------------
** init_fd_select - добавление сокета в сет файловых
**					дескрипотов для селекта
** ----------------------------------------------------------
*/

void IRC::
init_fd_select()
{
	FD_ZERO(&_fd_set_read);
	FD_ZERO(&_fd_set_write);
	for (map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end(); it++)
		FD_SET(it->first, &_fd_set_read);
	while (!_command_queue.empty())
	{
		if (_array_fd_select.count(_command_queue.front().first))
		{
			FD_SET(_command_queue.front().first, &_fd_set_write);
			break;
		}
		else
			_command_queue.pop();
	}
}

/*
** ----------------------------------------------------------
** do_select - селект!
** ----------------------------------------------------------
*/

void IRC::
do_select()
{
	if ((_select_res = select(FD_SETSIZE, &_fd_set_read, &_fd_set_write, NULL, NULL)) < 0)
	{
		utils::print_error(ERR_SELECT, "SELECT");
	}
}

/*
** -----------------------------------------------------------------------------------
** check_fd_select - проходимся по всем дескрипторам, если встречаем тот, который
** 					 содержится в отслеживаемых селектом (т.е. он содержится в сете);
**					 - Если это дескриптор сервера, то нужно подтвердить соединение и
**		  			   установить тип этого дескриптора, как клиентский;
**					 - Если это дескриптор клиента - то нужно получить от него 
**					   сообщение и отослать серверу;		 
** -----------------------------------------------------------------------------------
*/

void IRC::
check_fd_select()
{
	for (map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end() && _select_res > 0; it++)
	{
		if (FD_ISSET(it->first, &_fd_set_write))
		{
			int server_el = IRC::find_fd(_servers, it->first);
			int client_el = IRC::find_fd(_clients, it->first);

			if (server_el >= 0)
			{
				_servers[server_el]->getStatistics().sent(_command_queue.front().second);
				_servers[server_el]->getStatistics().queued(_command_queue.front().second, false);
			}
			else
			{
				_clients[client_el]->getStatistics().sent(_command_queue.front().second);
				_clients[client_el]->getStatistics().queued(_command_queue.front().second, false);
			}

			_send(it->second, it->first, _command_queue.front().second.c_str(), strlen(_command_queue.front().second.c_str()), 0);
			_command_queue.pop();
		}

		if (FD_ISSET(it->first, &_fd_set_read))
		{
			if (it->second == FD_CLIENT || it->second == FD_CLIENT_SSL)
			{
				char buffer[512 + 1];
				int n = _recv(it->second, it->first, buffer, 512, 0);
				if (!strcmp(buffer, "\n"))
				{
					buffer[0] = '\0';
					continue;
				}
				if (n < 1)
				{
					it = _array_fd_select.begin();
					continue;
				}
				buffer[n] = '\0';;
				//получаем распарсенный вектор команд если нашли \r\n
				vector<string> buffer_cmd = check_buffer(it->first, buffer);

				if (buffer_cmd.size())
					utils::print_message(it->first, buffer_cmd);
				else
					std::cout << "Client " << it->first << " " << buffer << std::endl;

				for (size_t i = 0; i < buffer_cmd.size(); i++)
				{
					Command mess(buffer_cmd[i]);
					do_command(&mess, it->first); // передаем в исполнение команды сообщение и сокет, из которого пришло сообщение
				}
				bzero(buffer, 512);
				break;
			}
			//Раздел для приемки сообщений из селекта
			if (it->second == FD_SERVER || it->second == FD_SERVER_SSL)
			{
				// _accept() возвращает fd клиента, который мы добавляем в map
				// в качестве ключа, в качестве значения добавляем FD_CLIENT(_SSL)

				int client_socket = (it->second == FD_SERVER)
									? _localhost._accept()
									: _localhost_ssl._accept();

				_array_fd_select[client_socket] = (it->second == FD_SERVER)
																? FD_CLIENT
																: FD_CLIENT_SSL;
				if (it->second == FD_SERVER_SSL)
					ssl_connection(client_socket);

				utils::print_client(client_socket, "New client joined server!");
				Client *new_client;
				if (it->second == FD_SERVER)
					new_client = new Client(_localhost.getHostname(), client_socket);
				else
					new_client = new Client(_localhost_ssl.getHostname(), client_socket);
				_clients.push_back(new_client);
			}
			_select_res--;
		}
	}
}

int IRC::
_send(int connection_type, int fd, const char* response, size_t size, int flags)
{
	int n;

	if (connection_type == FD_CLIENT || connection_type == FD_SERVER)
		n = send(fd, reinterpret_cast<const void*>(response), size, flags);
	else
		n = SSL_write(_ssl, reinterpret_cast<const void*>(response), size);
	
	if (n < 0)
		std::cout << "message sending failed\n";
	
	return n;
}

int IRC::
_recv(int connection_type, int fd, char* response, size_t size, int flags)
{
	int n;

	if (connection_type == FD_CLIENT || connection_type == FD_SERVER)
		n = recv(fd, reinterpret_cast<void*>(response), size, flags);
	else
		n = SSL_read(_ssl, reinterpret_cast<void*>(response), size);
	
	if (n == 0 || n < 0)
	{
		if (find_fd(_servers, fd) >= 0)
		{
			std::stringstream out_mess;
			out_mess << ":" << _servers[find_fd(_servers, fd)]->getName()
					 << " SQUIT " << _servers[find_fd(_servers, fd)]->getName()
					 << " :Error accured\r\n";
			forward_to_servers(fd, out_mess.str());
		}
		else if (find_fd(_users, fd) >= 0)
		{
			std::stringstream out_mess;
			out_mess << ":" << _users[find_fd(_users, fd)]->getName()
					 << " QUIT :Client closed connection\r\n";
			forward_to_servers(fd, out_mess.str());
		}
		close_connection(fd, n);
	}
	return n;
}

void IRC::close_connection(int fd, int n)
{
	if (n)
		utils::print_line("message receiving failed");
	if (_array_fd_select.count(fd))
	{
		Client* user;

		if (is_server(fd))
			close_connection(get_server(fd));
		else if ((user = get_user(fd)))
			close_connection(get_user(fd));
		else if ((user = get_client(fd)))
			close_connection(get_client(fd));
		else
		{
			_array_fd_select.erase(fd);
			close(fd);
		}	
	}
}

void IRC::
close_connection(Client* client)
{
	int fd = client->getSocketFd();
	_array_fd_select.erase(fd);
	close(fd);
	delete_client(client);
}

void IRC::
close_connection(User* user)
{
	int fd = user->getSocketFd();
	if (!is_server(fd))
	{
		_array_fd_select.erase(fd);
		close(fd);
	}
	delete_user(user);
}

void IRC::
close_connection(Server* server)
{
	if (!server) return;

	delete_client(get_client(server));		//TODO проверить, удаялются ли серверы из _clients
	
	if (server->getHopcount() == 1)
	{
		int fd = server->getSocketFd();
		_array_fd_select.erase(fd);
		close(fd);
		utils::print_line("Connection with " + server->getName() + " terminated");
	}
	
	vector<Server*>::iterator it = find(_servers.begin(), _servers.end(), server);
	if (it != _servers.end())
	{
		_servers.erase(it);
		utils::print_line("Server " + server->getName() + " deleted from server list");
	}
	delete server;
}

/*
** ========================SSL PART==========================
**
** ----------------------------------------------------------
** init_ctx - инициализация конфигурации для SSL
** ----------------------------------------------------------
*/

void IRC::
init_ssl()
{
	SSL_library_init();					/*load encryption and hash algo's in ssl*/
	OpenSSL_add_all_algorithms();		/* load & register all cryptos, etc.*/
	ERR_load_crypto_strings();		
	SSL_load_error_strings();			/* load all error messages */
}

void IRC::
init_ctx()
{
	const SSL_METHOD* method;

	method = TLS_server_method();	/* create new server-method instance */

	if (!(_ctx = SSL_CTX_new(method)))	/* create new context from method */
		utils::print_error(1, "SSL: SSL_CTX_new failed\n");

	if (SSL_CTX_use_certificate_file(_ctx, CERTIFICATE, SSL_FILETYPE_PEM) <= 0)
		utils::print_error(1, "SSL: SSL_CTX_use_certificate_file failed\n");
	if (SSL_CTX_use_PrivateKey_file(_ctx, PRIVATE_KEY, SSL_FILETYPE_PEM) <= 0)
		utils::print_error(1, "SSL: SSL_CTX_use_PrivateKey_file SSL failed\n");
	if (!SSL_CTX_check_private_key(_ctx))
		utils::print_error(1, "SSL: SSL_CTX_check_private_key failed\n");
}

SSL* IRC::
ssl_connection(int fd)
{
	if(!(_ssl = SSL_new(_ctx)))
		utils::exit_error(1, "SSL: init failed\n");

	if (SSL_set_fd(_ssl, fd) <= 0)
		utils::exit_error(1, "SSL: SSL_set_fd failed\n");

	if (SSL_accept(_ssl) < 0)		/* do SSL-protocol accept */
		utils::exit_error(1, "SSL: SSL_accept failed\n");

	return _ssl;
}

void IRC::
add_network_ip(const string& ip)
{
	_network_ip.push_back(ip);
}

void IRC::
add_network_port(const string& port)		
{
	_network_port.push_back(std::atoi(port.c_str()));
}

void IRC::
add_network_pass(const string& pass)
{
	_network_pass.push_back(pass);
}

void IRC::
add_fd(int fd, int fd_type)			{ _array_fd_select[fd] = fd_type; }

/*
** ===========================utils PART=============================
** ------------------------------------------------------------------
** delete_user		- находит и удаляет клиента или юзера с 
** delete_client      соответствующим fd
** ------------------------------------------------------------------
*/

/*
** ВАЖНО
** когда к нам добавятся юзеры из других серверов, так удалять будет нельзя!
** нужно будет смотреть на префиксы, а не только на fd
*/

// NEED_TO_DELETE
// void IRC::
// delete_user(int fd)
// {
// 	int i;
// 	if ((i = IRC::find_fd(_users, fd)) > -1)
// 	{
// 		User* out_user = _users[i];
// 		_users.erase(_users.begin() + i);
// 		delete out_user;
// 	}
// }

/*
** ВАЖНО
** когда к нам добавятся юзеры из других серверов, так удалять будет нельзя!
** нужно будет смотреть на префиксы, а не только на fd
*/

void IRC::
delete_client(int fd)
{
	int i;
	if ((i = IRC::find_fd(_clients, fd)) > -1)
	{
		Client* out_client = _clients[i];
		_clients.erase(_clients.begin() + i);
		delete out_client;
	}
}

void IRC::
delete_channel(string name)			{ _channels.erase(name); }

/*
** ----------------------------------------------------------
** check_buffer - возвращает расспарсенный вектор буфера с соединенными командами
** ----------------------------------------------------------
*/

vector<string> IRC::
check_buffer(int fd, const char* buffer)
{
	Client* ptr_client;
	Server* ptr_server;
	vector<string> temp_vec;
	int pos_client = -1;
	int pos_server = -1;
	string full_buffer;

	if ((pos_server = IRC::find_fd(_servers, fd)) == -1 &&
		(pos_client = IRC::find_fd(_clients, fd)) == -1)
	{
		utils::print_error(ERR_FINDFD, "In function check_buffer didn't find fd");
		return (temp_vec);
	}

	if (pos_server != -1)
	{
		ptr_server = _servers[pos_server];
		full_buffer = ptr_server->getBuffer() + static_cast<string>(buffer);
		ptr_server->setBuffer(full_buffer);
		while (ptr_server->find_line_break())
		{
			string temp_str = ptr_server->get_line_break();
			if (is_equal(temp_str, "CAP LS") || is_equal(temp_str, ""))
				continue;
			temp_vec.push_back(temp_str);
		}
	}
	else
	{
		ptr_client = _clients[pos_client];
		full_buffer = ptr_client->getBuffer() + static_cast<string>(buffer);
		ptr_client->setBuffer(full_buffer);
		while (ptr_client->find_line_break())
		{
			string temp_str = ptr_client->get_line_break();
			if (is_equal(temp_str, "CAP LS") || is_equal(temp_str, ""))
				continue;
			temp_vec.push_back(temp_str);
		}
	}
	return (temp_vec);
}

/*
** ----------------------------------------------------------
** push_cmd_queue - пушит строку в очередь
** ----------------------------------------------------------
*/
void IRC::
push_cmd_queue(int fd, const string& str)
{
	int server_el = IRC::find_fd(_servers, fd);
	int client_el = IRC::find_fd(_clients, fd);

	if (server_el >= 0)
		_servers[server_el]->getStatistics().queued(str, true);
	else
		_clients[client_el]->getStatistics().queued(str, true);

	std::cout << CYAN << "QUEUE #" << fd << ": " << YELLOW << str.substr(0, str.size() - 2) << RESET << std::endl;
	_command_queue.push(std::make_pair(fd, str));
}

User* IRC::
get_user(string nickname)
{
	int index = find_name(_users, nickname);

	return (index >= 0) ? _users[index] : NULL;
}

User* IRC::
get_user(int fd)
{
	int index = find_fd(_users, fd);

	return (index >= 0) ? _users[index] : NULL;
}

vector<User*>& IRC::
get_users() 								{ return _users; }

vector<Client*>& IRC::
get_clients()								{ return _clients; }

Client* IRC::
get_client(int fd)							{ return _clients[find_fd(_clients, fd)]; }

Client* IRC::
get_client(User* user)
{
	int i = find_name(_clients, user->getName());
	return (i >= 0) ? _clients[i] : NULL;
}

Client* IRC::
get_client(Server* server)
{
	////////////////////////////////// TODO неправильно ищется
	int i = find_fd(_clients, server->getSocketFd());
	return (i >= 0) ? _clients[i] : NULL;
}

User* IRC::
get_user(Client *client)
{
	int i = IRC::find_name(_users, client->getName());
	return (i >=0) ? _users[i] : NULL;
}

vector<Server*>& IRC::
get_servers()								{ return _servers; }

Server* IRC::
get_server(int fd)
{
	int n = find_fd(_servers, fd);
	return (n >= 0) ? _servers[n] : NULL;
}

Server* IRC::
get_server(const string& name) const
{
	int n = find_name(_servers, name);
	return (n >= 0) ? _servers[n] : NULL;
}

const string& IRC::
get_server_name()							{ return _server_name; }

const string& IRC::
get_server_name(int fd)						{ return _servers[find_fd(_servers, fd)]->getName(); }

size_t IRC::
get_server_count()							{ return _network_ip.size(); }

int IRC::
get_localhost_port() const					{ return _localhost.get_port(); }

const string& IRC::
get_localhost_pass() const					{ return _localhost_pass; }

const string& IRC::
get_operator_user() const					{ return _operator_user; }

const string& IRC::
get_operator_pass() const					{ return _operator_pass; }

const Socket& IRC::
get_socket() const							{ return _localhost; }

channel_map& IRC::
get_channels()								{ return _channels; }

Channel* IRC::
get_channel(string channel_name) {

	if (!(channel_name[0] == '&' || channel_name[0] == '#'))
		return NULL;
	
	channel_map& channels = get_channels();
	channel_map::iterator it = channels.find(channel_name);
	return (it != channels.end())
			? &(it->second)
			: NULL;
}

Statistics & IRC::
get_statistics()							{ return _statistics; }

vector<Socket>& IRC::
get_network()								{ return _network; }

const string& IRC::
get_nickname(int fd)						{ return _users[find_fd(_users, fd)]->getName(); }

bool IRC::
is_empty_queue() const						{ return (_command_queue.empty());}

bool IRC::
is_server(int fd) const						{ return (IRC::find_fd(_servers, fd)) >= 0; }

bool IRC::
is_server_operator(const User* user) const 	{ return user->getMode().o; }

bool IRC::
is_numeric_response(const Command& command)
{
	if ( 								// пересылаем сообщения
		command.getCommand() == "256" ||
		command.getCommand() == "257" ||
		command.getCommand() == "258" ||
		command.getCommand() == "259" ||
		command.getCommand() == "259" ||
		command.getCommand() == "211" ||
		command.getCommand() == "212" ||
		command.getCommand() == "219" ||
		command.getCommand() == "421" ||
		command.getCommand() == "371" ||
		command.getCommand() == "374" ||
		command.getCommand() == "351" ||
		command.getCommand() == "005" ||
		command.getCommand() == "242" ||
		command.getCommand() == "364" ||
		command.getCommand() == "365" ||
		command.getCommand() == "200" ||
		command.getCommand() == "201" ||
		command.getCommand() == "202" ||
		command.getCommand() == "203" ||
		command.getCommand() == "204" ||
		command.getCommand() == "205" ||
		command.getCommand() == "206" ||
		command.getCommand() == "208" ||
		command.getCommand() == "262" ||
		command.getCommand() == "311" ||
		command.getCommand() == "312" ||
		command.getCommand() == "313" ||
		command.getCommand() == "317" ||
		command.getCommand() == "318"
		)
	{
		const vector<string>& args = command.getArgs();
		User* usr;

		if (!args.empty() && ((usr = get_user(args[0]))))
		{
			push_cmd_queue(usr->getSocketFd(), command.getMessage() + "\r\n");
			return true;
		}
	}
	if (command.getCommand() == "421" ||			// игнорируем сообщение "Неизвестная команда"
		command.getCommand() == "451" ||			// игнорируем сообщение "Вы не зарегистрированы"
		command.getCommand() == "436")				// игнорируем сообщение "Nick collision"
		return true;
	return false;
}

string IRC::
response(int response_code, string name, string command, string message)
{
	string code 		= utils::int_to_str(response_code);
	command = command.size() ? command + " " : command;

	string response = ":"
					+ _server_name + " "
					+ code + " "
					+ name + " "
					+ command
					+ message
					+ "\r\n";
	return response;
}

string IRC::
response(int response_code, int client_fd, string command, string message)
{
	string code = utils::int_to_str(response_code);
	command = command.size() ? command + " " : command;

	int i = find_fd(_clients, client_fd);
	string client_name = (i >= 0) ? _clients[i]->getName() : string();
	if (client_name.empty())		// если не нашли среди клиентов, ищем среди серверов
	{
		i = find_fd(_servers, client_fd);
		client_name = (i >= 0) ? _servers[i]->getName() : string();
	}

	string response = ":"
					+ _server_name + " "
					+ code + " "
					+ client_name + " "
					+ command
					+ message
					+ "\r\n";
	return response;
}

void IRC::
forward_to_servers(int fd, const string& message)
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i]->getSocketFd() != fd && _servers[i]->getHopcount() == 1)
			push_cmd_queue(_servers[i]->getSocketFd(), message + "\r\n");
	}
}

void IRC::
forward_to_clients(IRC& irc, const string& message)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (!irc.is_server(_clients[i]->getSocketFd()))
			push_cmd_queue(_clients[i]->getSocketFd(), message + "\r\n");
	}
}

void IRC::
forward_to_channel(int fd, Channel& channel, const string& message)
{
	user_map& users = channel.get_users();

	for (user_iterator it = users.begin(); it != users.end(); it++)
	{
		if (it->first->getHopcount() == 0 && it->first->getSocketFd() != fd)
			push_cmd_queue(it->first->getSocketFd(), message + "\r\n");
	}
}

void IRC::
forward_to_channel(int fd, const string& channel_name, const string& message)
{
	Channel& channel = _channels[channel_name];
	forward_to_channel(fd, channel, message);
}

void IRC::
forward_to_all_channels(User* user, const string& message)
{
	for (channel_iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		user_map users = it->second.get_users();
		if (users.count(user))
			forward_to_channel(user->getSocketFd(), it->first, message);
	}
}

string IRC::
fullname(const User* user) const
{
	string fullname = ":" + user->getName() + "!~" + user->getUsername() + "@" + user->getHostname();
	return fullname;
}

// DEBUG
// выводит список существующих на сервере каналов и пользователей на них

void IRC::print_channels() const {
	for (channel_map::const_iterator it = _channels.begin(); it != _channels.end(); it++) {
		std::cout << "\tChannel: " << it->first << std::endl;
		it->second.print_users();
	}
}


void IRC::
delete_user(User* usr)
{
	delete_client(get_client(usr));
	delete_user_from_channels(usr, string(" QUIT :Client closed connection"));

	vector<User*>::iterator it = find(_users.begin(), _users.end(), usr);
	if (it != _users.end())
		_users.erase(it);
	delete usr;
}

void IRC::
delete_client(Client* clnt)
{
	if (!clnt) return;
	
	vector<Client*>::iterator it = find(_clients.begin(), _clients.end(), clnt);
	if (it != _clients.end())
		_clients.erase(it);
	delete clnt;
}

#include <list>
void IRC::
delete_user_from_channels(User* user, const string& quit_mess)
{
	int fd = user->getSocketFd();
	channel_map& _channels = get_channels();

	//удаляем юзера со всех каналов текущего сервера
	channel_map::iterator it_channels = _channels.begin();

	while (it_channels != _channels.end() && user->getChannelCount())
	{
		user_map& chann_users = it_channels->second.get_users();	// список пользователей канала

		if (chann_users.count(user))
		{
			chann_users.erase(user);
			user->dec_channel_count();
			if (chann_users.size() == 0) // если канал пуст удалить его из мапы
			{
				_channels.erase(it_channels);
				it_channels = _channels.begin();
				continue;
			}
			else //иначе отправить всем пользователем канал сообщение о выходе юзера
				forward_to_channel(fd, it_channels->first, fullname(user) + quit_mess);

			forward_to_servers(fd, ":" + user->getName() + quit_mess);
		}
		// удаляем из бан списка
		vector<User*>::iterator it_ban_start = it_channels->second.getBanned().begin();
		vector<User*>::iterator it_ban_end = it_channels->second.getBanned().end();

		while (it_ban_start != it_ban_end)
		{
			if ((*it_ban_start)->getNickname() == user->getNickname())
			{
				it_channels->second.getBanned().erase(it_ban_start);
				break;
			}
			++it_ban_start;
		}
		++it_channels;
	}
}

vector<string> IRC::
motd_generate()
{
	vector<string> out_mess_to_user;
	out_mess_to_user.push_back("****************************************************************************");
	out_mess_to_user.push_back("*_      `-._     `-.     `.   \\      :      /   .'     .-'     _.-'      _ *");
	out_mess_to_user.push_back("* `--._     `-._    `-.    `.  `.    :    .'  .'    .-'    _.-'     _.--'  *");
	out_mess_to_user.push_back("*      `--._    `-._   `-.   `.  \\   :   /  .'   .-'   _.-'    _.--'       *");
	out_mess_to_user.push_back("*`--.__     `--._   `-._  `-.  `. `. : .' .'  .-'  _.-'   _.--'     __.--' *");
	out_mess_to_user.push_back("*__    `--.__    `--._  `-._ `-. `. \\:/ .' .-' _.-'  _.--'    __.--'    __ *");
	out_mess_to_user.push_back("*  `--..__   `--.__   `--._ `-._`-.`_=_'.-'_.-' _.--'   __.--'   __..--'   *");
	out_mess_to_user.push_back("*--..__   `--..__  `--.__  `--._`-q(-_-)p-'_.--'  __.--'  __..--'   __..-- *");
	out_mess_to_user.push_back("*      ``--..__  `--..__ `--.__ `-'_) (_`-' __.--' __..--'  __..--''       *");
	out_mess_to_user.push_back("*...___        ``--..__ `--..__`--/__/  \\--'__..--' __..--''        ___... *");
	out_mess_to_user.push_back("*      ```---...___    ``--..__`_(<_   _/)_'__..--''    ___...---'''       *");
	out_mess_to_user.push_back("*```-----....._____```---...___(__\\_\\_|_/__)___...---'''_____.....-----''' *");
	out_mess_to_user.push_back("* ___   __  ________   _______   _       _   _______    ___   __   _______ *");
	out_mess_to_user.push_back("*|| \\  ||     ||     ||_____))  \\\\     //  ||_____||  || \\\\  ||  ||_____|| *");
	out_mess_to_user.push_back("*||  \\_||  ___||___  ||     \\\\   \\\\___//   ||     ||  ||  \\\\_||  ||     || *");
	out_mess_to_user.push_back("****************************************************************************");
	return (out_mess_to_user);
}
