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
									  "STATS"};

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

CmdStats::
CmdStats() {}

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
	_network_ip = network_ip;
	_network_port = std::atoi(network_port.c_str());
	_network_pass = network_pass;
	_localhost_pass = localhost_pass;
	int port = std::atoi(localhost_port.c_str());
	_localhost = Socket(LOCALHOST, port);
	_localhost_ssl = Socket(LOCALHOST, port + 1);
	this->generate_map_codes();
	utils::print_line("Constructor IRC done!");
	utils::print_line("Socket local done!");
	generate_map_cmd_stats();
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
create_socket_network(std::vector<std::string> network)
{
	_network = Socket(_network_ip.c_str(), _network_port);
	utils::print_line("Socket network done!");
	int fd = _network._socket();
	utils::print_line("Socket network FD done!");
	_network._connect();
	utils::print_line("Socket network connection!");

	_array_fd_select[fd] = FD_CLIENT;
	_clients.push_back(new Client(network[0], fd));
	//_servers.push_back(new Server(fd, network[0] + "/" + network[1], 1, "info"));
	utils::print_line("Trying to connect to server!\nServer name: " +
					  network[0] + "/" + network[1] +
					  "\nHopcount: 1\nInfo: info");
	push_cmd_queue(fd, "PASS " + _network_pass + "\r\nSERVER " + _server_name + " 1 info\r\n");
	_clients[0]->setIsServer(true);
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

typedef int (Command::*doCommand)(IRC& irc, int fd);
typedef void (IRC::*SignalHandlerPointer)(int);

int IRC::
do_command(Command* command, int fd)
{
	int			result = 123;
	string cmd_name[COMM_COUNT] =  {"NICK",
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
									"STATS"
									};
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
										&Command::cmd_stats
										};

	const string & comm 			= command->getCommand();
	int client_el					= IRC::find_fd(_clients, fd);
	int server_el					= IRC::find_fd(_servers, fd);

	if (server_el >= 0)
		_servers[server_el]->getStatistics().recieved(comm, command->getMessage(), map_cmd_stats);
	else if (client_el >= 0)
		_clients[client_el]->getStatistics().recieved(comm, command->getMessage(), map_cmd_stats);

	if (!(is_equal(comm, "PASS")
		|| (client_el >= 0 && _clients[client_el]->getPassword()
			&& (is_equal(comm, "NICK") || is_equal(comm, "USER") || is_equal(comm, "SERVER")))	// client entered correct pass
		|| IRC::find_fd(_servers, fd) >= 0								// client is registred as Server
		|| IRC::find_fd(_users, fd) >= 0))								// client is registred as User
	{
		string message = ":" + _server_name + " " +
						 utils::int_to_str(ERR_NOTREGISTERED) + " "
						 "*" + ERR_NOTREGISTERED_MESS + "\r\n";
		this->push_cmd_queue(fd, message);
		return 0;
	}

	for (int i = 0; i < COMM_COUNT; i++)
	{
		if (is_equal(cmd_name[i], comm))
		{
			utils::print_command(fd, command->getMessage());
			result = (command->*(cmd_func[i]))(*this, fd);
			return (result);
		}
	}
	this->push_cmd_queue(fd, this->response(ERR_UNKNOWNCOMMAND, fd, comm, ERR_UNKNOWNCOMMAND_MESS));
	return 0;
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
	if (!_command_queue.empty())
	{
		FD_SET(_command_queue.front().first, &_fd_set_write);
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
		utils::print_error(ERR_SELECT, "SELECT");
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
			// std::cout << "DEBUG: Сообщение отправлено клиенту: " << _command_queue.front().second.substr(0, _command_queue.front().second.size() - 1) << std::endl;
			
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
				vector<string> buffer_cmd = this->check_buffer(it->first, buffer);

				utils::print_message(it->first, buffer_cmd);

				for (size_t i = 0; i < buffer_cmd.size(); i++)
				{
					Command mess(buffer_cmd[i]);
					this->do_command(&mess, it->first); // передаем в исполнение команды сообщение и сокет, из которого пришло сообщение
				}
				bzero(buffer, 512);
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
				this->_clients.push_back(new_client);
				std::cout << "DEBUG client information:\n"
						  << "Socket FD: " << new_client->getSocketFd() << std::endl
						  << "Hostname: " << new_client->getHostname() << std::endl;
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
		this->close_connection(fd, n);
	return n;
}

void IRC::close_connection(int fd, int n)
{
	if (n)
		utils::print_line("message receiving failed");
		
	if (is_server(fd))
		close_connection(get_server(fd));
	else
		close_connection(get_user(fd));
}

void IRC::
close_connection(User* user)
{
	if (!user) return;

	int fd = user->getSocketFd();
	Client* client = get_client(user);

	if (!is_server(fd))
	{
		_array_fd_select.erase(fd);
		close(fd);
	}
	
	vector<User*>::iterator it1 = find(_users.begin(), _users.end(), user);
	_users.erase(it1);

	vector<Client*>::iterator it2 = find(_clients.begin(), _clients.end(), client);
	_clients.erase(it2);

	delete user;
	delete client;

	utils::print_line("connection closed");
}

void IRC::
close_connection(Server* server)
{
	if (!server) return;

	int fd = server->getSocketFd();
	Client* client = get_client(server);

	_array_fd_select.erase(fd);
	close(fd);
	
	vector<Server*>::iterator it1 = find(_servers.begin(), _servers.end(), server);
	_servers.erase(it1);

	vector<Client*>::iterator it2 = find(_clients.begin(), _clients.end(), client);
	_clients.erase(it2);

	delete server;
	delete client;

	utils::print_line("connection closed");
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

void IRC::
delete_user(int fd)
{
	int i;
	if ((i = IRC::find_fd(this->_users, fd)) > -1)
	{
		User* out_user = this->_users[i];
		this->_users.erase(this->_users.begin() + i);
		delete out_user;
	}
}

/*
** ВАЖНО
** когда к нам добавятся юзеры из других серверов, так удалять будет нельзя!
** нужно будет смотреть на префиксы, а не только на fd
*/

void IRC::
delete_client(int fd)
{
	int i;
	if ((i = IRC::find_fd(this->_clients, fd)) > -1)
	{
		Client* out_client = this->_clients[i];
		this->_clients.erase(this->_clients.begin() + i);
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

	if ((pos_server = IRC::find_fd(this->_servers, fd)) == -1 &&
		(pos_client = IRC::find_fd(this->_clients, fd)) == -1)
	{
		utils::print_error(ERR_FINDFD, "In function check_buffer didn't find fd");
		return (temp_vec);
	}

	if (pos_server != -1)
	{
		ptr_server = this->_servers[pos_server];
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
		ptr_client = this->_clients[pos_client];
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
		_servers[server_el]->getStatistics().sent(str);
	else
		_clients[client_el]->getStatistics().sent(str);
	std::cout << CYAN << "QUEUE #" << fd << ": " << YELLOW << str.substr(0, str.size() - 2) << RESET << std::endl;
	this->_command_queue.push(std::make_pair(fd, str));
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
get_users() 				{ return _users; }

vector<Client*>& IRC::
get_clients()				{ return _clients; }

Client* IRC::
get_client(User* user)
{
	int i = find_name(_clients, user->getName());
	return (i >= 0) ? _clients[i] : NULL;
}

Client* IRC::
get_client(Server* server)
{
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
get_servers()				{ return _servers; }

map<string, CmdStats> &IRC::
get_map_cmd_stats()			{ return map_cmd_stats; }

Server* IRC::
get_server(int fd)			{ return _servers[find_fd(_servers, fd)]; }

const string& IRC::
get_server_name()			{ return _server_name; }

const string& IRC::
get_server_name(int fd)		{ return _servers[find_fd(_servers, fd)]->getName(); }

int IRC::
get_localhost_port() const	{ return _localhost.get_port(); }

const string& IRC::
get_localhost_pass() const	{ return _localhost_pass; }

const string& IRC::
get_operator_user() const	{ return _operator_user; }

const string& IRC::
get_operator_pass() const	{ return _operator_pass; }

const Socket& IRC::
get_socket() const			{ return _localhost; }

channel_map& IRC::
get_channels()				{ return _channels; }

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

const string& IRC::
get_nickname(int fd)						{ return _users[find_fd(_users, fd)]->getName(); }

bool IRC::
is_empty_queue() const						{ return (this->_command_queue.empty());}

bool IRC::
is_server(int fd) const						{ return (IRC::find_fd(_servers, fd)) >= 0; }

bool IRC::
is_server_operator(const User* user) const 	{ return user->getMode().o; }

string IRC::
response_2(int response_code, int fd, string command, string message)
{
	string code 		= utils::int_to_str(response_code);
	string server_name	= _servers[find_fd(_servers, fd)]->getName();

	string response = ":"
					+ _server_name + " "
					+ code + " "
					+ server_name + " "
					+ command
					+ message
					+ "\r\n";
	return response;
}

string IRC::
response_3(int response_code, string name, string command, string message)
{
	string code 		= utils::int_to_str(response_code);

	string response = ":"
					+ _server_name + " "
					+ code + " "
					+ name + " "
					+ command + " "
					+ message
					+ "\r\n";
	return response;
}

string IRC::
response(int response_code, int client_fd, string message_prefix, string message)
{
	string code = utils::int_to_str(response_code);

	int i = find_fd(_clients, client_fd);
	string client_name = (i >= 0) ? _clients[i]->getName() : string();

	string response = ":"
					+ _server_name + " "
					+ code + " "
					+ client_name + " "
					+ message_prefix
					+ message
					+ "\r\n";
	return response;
}

void IRC::
forward_to_servers_2(int fd, const string& prefix, const string& message)
{
	string forward_mess = prefix.empty() ? (message)
										 : (":" + prefix + " " + message);
	
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i]->getSocketFd() != fd && _servers[i]->getHopcount() == 1)
			push_cmd_queue(_servers[i]->getSocketFd(), forward_mess + "\r\n");
	}
}

void IRC::
forward_to_servers(int fd, const string& message, bool prefix)
{
	string forward_mess = prefix ? message : (":" + get_nickname(fd) + " " + message);
	
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i]->getSocketFd() != fd && _servers[i]->getHopcount() == 1)
			push_cmd_queue(_servers[i]->getSocketFd(), forward_mess + "\r\n");
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

string IRC::
full_name(const User* user) const
{
	string fullname = ":" + user->getName() + "!~" + user->getUsername() + "@" + user->getHostname();
	return fullname;
}

int	IRC::
push_mess_client(int fd, int code)
{
	std::map<int, std::string>::const_iterator it = map_codes.find(code);
	if (it == map_codes.end())
		utils::print_error(code, "Unknown error");
	else
	{
		this->push_cmd_queue(fd, this->response(code, fd, utils::int_to_str(code), it->second));	
	}
	return (code);
}

/*
**==========================
** generate_map_codes - генерирует стандартные коды ошибок в мапу map_codes
**==========================
*/


void IRC::
generate_map_cmd_stats()
{
	string cmd_name[COMM_COUNT] = {"NICK",
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
								   "STATS"};

	for (int i = 0; i < COMM_COUNT; i++)
	{
		map_cmd_stats.insert(std::make_pair<string, CmdStats>(cmd_name[i], CmdStats(cmd_name[i])));
	}
}

void IRC::
generate_map_codes()
{
	map_codes.insert(std::make_pair<int, std::string>(401, " :No such nick/channel"));
	map_codes.insert(std::make_pair<int, std::string>(402, " :No such server"));
	map_codes.insert(std::make_pair<int, std::string>(403, " :No such channel"));
	map_codes.insert(std::make_pair<int, std::string>(404, " :Cannot send to channel"));
	map_codes.insert(std::make_pair<int, std::string>(405, " :You have joined too many channels"));
	map_codes.insert(std::make_pair<int, std::string>(406, " :There was no such nickname"));
	map_codes.insert(std::make_pair<int, std::string>(407, " :Duplicate recipients. No message delivered"));
	map_codes.insert(std::make_pair<int, std::string>(409, " :No origin specified"));
	map_codes.insert(std::make_pair<int, std::string>(411, " :No recipient given (<command>)"));
	map_codes.insert(std::make_pair<int, std::string>(412, " :No text to send"));
	map_codes.insert(std::make_pair<int, std::string>(413, " :No toplevel domain specified"));
	map_codes.insert(std::make_pair<int, std::string>(414, " :Wildcard in toplevel domain"));
	map_codes.insert(std::make_pair<int, std::string>(421, " :Unknown command" ));
	map_codes.insert(std::make_pair<int, std::string>(422, " :MOTD File is missing"));
	map_codes.insert(std::make_pair<int, std::string>(423, " :No administrative info available" ));
	map_codes.insert(std::make_pair<int, std::string>(424, " :File error doing <file op> on <file>"));
	map_codes.insert(std::make_pair<int, std::string>(431, " :No nickname given"));
	map_codes.insert(std::make_pair<int, std::string>(432, " :Erroneus nickname"));
	map_codes.insert(std::make_pair<int, std::string>(433, " :Nickname is already in use"));
	map_codes.insert(std::make_pair<int, std::string>(436, " :Nickname collision KILL"));
	map_codes.insert(std::make_pair<int, std::string>(441, " :They aren't on that channel"));
	map_codes.insert(std::make_pair<int, std::string>(442, " :You're not on that channel"));
	map_codes.insert(std::make_pair<int, std::string>(443, " :is already on channel"));
	map_codes.insert(std::make_pair<int, std::string>(444, " :User not logged in"));
	map_codes.insert(std::make_pair<int, std::string>(445, " :SUMMON has been disabled"));
	map_codes.insert(std::make_pair<int, std::string>(446, " :USERS has been disabled"));
	map_codes.insert(std::make_pair<int, std::string>(451, " :You have not registered"));
	map_codes.insert(std::make_pair<int, std::string>(461, " :Not enough parameters"));
	map_codes.insert(std::make_pair<int, std::string>(462, " :You may not reregister"));
	map_codes.insert(std::make_pair<int, std::string>(463, " :Your host isn't among the privileged"));
	map_codes.insert(std::make_pair<int, std::string>(464, " :Password incorrect"));
	map_codes.insert(std::make_pair<int, std::string>(465, " :You are banned from this server"));
	map_codes.insert(std::make_pair<int, std::string>(467, " :Channel key already set"));
	map_codes.insert(std::make_pair<int, std::string>(471, " :Cannot join channel (+l)"));
	map_codes.insert(std::make_pair<int, std::string>(472, " :is unknown mode char to me"));
	map_codes.insert(std::make_pair<int, std::string>(473, " :Cannot join channel (+i)"));
	map_codes.insert(std::make_pair<int, std::string>(474, " :Cannot join channel (+b)"));
	map_codes.insert(std::make_pair<int, std::string>(475, " :Cannot join channel (+k)"));
	map_codes.insert(std::make_pair<int, std::string>(481, " :Permission Denied- You're not an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(483, " :You cant kill a server!"));
	map_codes.insert(std::make_pair<int, std::string>(491, " :No O-lines for your host"));
	map_codes.insert(std::make_pair<int, std::string>(501, " :Unknown MODE flag"));
	map_codes.insert(std::make_pair<int, std::string>(502, " :Cant change mode for other users"));
	map_codes.insert(std::make_pair<int, std::string>(300, " :None"));
	map_codes.insert(std::make_pair<int, std::string>(302, " :[<reply>{<space><reply>}]"));
	map_codes.insert(std::make_pair<int, std::string>(303, " :[<nick> {<space><nick>}]"));
	map_codes.insert(std::make_pair<int, std::string>(301, " :<away message>"));
	map_codes.insert(std::make_pair<int, std::string>(305, " :You are no longer marked as being away"));
	map_codes.insert(std::make_pair<int, std::string>(306, " :You have been marked as being away"));
	map_codes.insert(std::make_pair<int, std::string>(311, " :<nick> <user> <host> * :<real name>"));
	map_codes.insert(std::make_pair<int, std::string>(312, " :<server info>"));
	map_codes.insert(std::make_pair<int, std::string>(313, " :is an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(317, " :seconds idle"));
	map_codes.insert(std::make_pair<int, std::string>(318, " :End of /WHOIS list"));
	map_codes.insert(std::make_pair<int, std::string>(319, " :{[@|+]<channel><space>}"));
	map_codes.insert(std::make_pair<int, std::string>(314, " :<nick> <user> <host> * :<real name>"));
	map_codes.insert(std::make_pair<int, std::string>(369, " :<nick> :End of WHOWAS"));
	map_codes.insert(std::make_pair<int, std::string>(321, " :Channel :Users  Name"));
	map_codes.insert(std::make_pair<int, std::string>(322, " :<channel> <# visible> :<topic>"));
	map_codes.insert(std::make_pair<int, std::string>(323, " :End of /LIST"));
	map_codes.insert(std::make_pair<int, std::string>(324, " :<channel> <mode> <mode params>"));
	map_codes.insert(std::make_pair<int, std::string>(331, " :No topic is set"));
	map_codes.insert(std::make_pair<int, std::string>(332, " :<topic>"));
	map_codes.insert(std::make_pair<int, std::string>(341, " :<channel> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(342, " :Summoning user to IRC"));
	map_codes.insert(std::make_pair<int, std::string>(351, " :<comments>"));
	map_codes.insert(std::make_pair<int, std::string>(352, " :<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"));
	map_codes.insert(std::make_pair<int, std::string>(315, " :End of /WHO list"));
	map_codes.insert(std::make_pair<int, std::string>(353, " :[[@|+]<nick> [[@|+]<nick> [...]]]"));
	map_codes.insert(std::make_pair<int, std::string>(366, " :End of /NAMES list"));
	map_codes.insert(std::make_pair<int, std::string>(364, " :<hopcount> <server info>"));
	map_codes.insert(std::make_pair<int, std::string>(365, " :End of /LINKS list"));
	map_codes.insert(std::make_pair<int, std::string>(367, " :<channel> <banid>"));
	map_codes.insert(std::make_pair<int, std::string>(368, " :End of channel ban list"));
	map_codes.insert(std::make_pair<int, std::string>(371, " :<string>"));
	map_codes.insert(std::make_pair<int, std::string>(374, " :End of /INFO list"));
	map_codes.insert(std::make_pair<int, std::string>(375, " :- <server> Message of the day - "));
	map_codes.insert(std::make_pair<int, std::string>(372, " :- <text>"));
	map_codes.insert(std::make_pair<int, std::string>(381, " :You are now an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(382, " :Rehashing"));
	map_codes.insert(std::make_pair<int, std::string>(391, " :<string showing server's local time>"));
	map_codes.insert(std::make_pair<int, std::string>(392, " :UserID   Terminal  Host"));
	map_codes.insert(std::make_pair<int, std::string>(393, " :%-8s %-9s %-8s"));
	map_codes.insert(std::make_pair<int, std::string>(394, " :End of users"));
	map_codes.insert(std::make_pair<int, std::string>(395, " :Nobody logged in"));
	map_codes.insert(std::make_pair<int, std::string>(200, " :Link <version & debug level> <destination> <next server>"));
	map_codes.insert(std::make_pair<int, std::string>(201, " :Try. <class> <server>"));
	map_codes.insert(std::make_pair<int, std::string>(202, " :H.S. <class> <server>"));
	map_codes.insert(std::make_pair<int, std::string>(203, " :[<client IP address in dot form>]"));
	map_codes.insert(std::make_pair<int, std::string>(204, " :Oper <class> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(205, " :User <class> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(206, " :Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"));
	map_codes.insert(std::make_pair<int, std::string>(208, " :<newtype> 0 <client name>"));
	map_codes.insert(std::make_pair<int, std::string>(261, " :File <logfile> <debug level>"));
	map_codes.insert(std::make_pair<int, std::string>(211, " :<linkname> <sendq> <sent messages> <sent bytes> <received messages> <received bytes> <time open>" ));
	map_codes.insert(std::make_pair<int, std::string>(212, " :<command> <count>"));
	map_codes.insert(std::make_pair<int, std::string>(213, " :C <host> * <name> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(214, " :N <host> * <name> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(215, " :I <host> * <host> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(216, " :K <host> * <username> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(218, " :Y <class> <ping frequency> <connect frequency> <max sendq>" ));
	map_codes.insert(std::make_pair<int, std::string>(219, " :End of /STATS report"));
	map_codes.insert(std::make_pair<int, std::string>(241, " :L <hostmask> * <servername> <maxdepth>"));
	map_codes.insert(std::make_pair<int, std::string>(242, " :Server Up %d days %d:%02d:%02d"));
	map_codes.insert(std::make_pair<int, std::string>(243, " :O <hostmask> * <name>"));
	map_codes.insert(std::make_pair<int, std::string>(244, " :H <hostmask> * <servername>"));
	map_codes.insert(std::make_pair<int, std::string>(221, " :<user mode string>"));
	map_codes.insert(std::make_pair<int, std::string>(251, " :There are <integer> users and <integer> invisible on <integer> servers"));
	map_codes.insert(std::make_pair<int, std::string>(252, " :operator(s) online"));
	map_codes.insert(std::make_pair<int, std::string>(253, " :unknown connection(s)"));
	map_codes.insert(std::make_pair<int, std::string>(254, " :channels formed"));
	map_codes.insert(std::make_pair<int, std::string>(255, " :I have <integer> clients and <integer> servers"));
	map_codes.insert(std::make_pair<int, std::string>(256, " :Administrative info"));
	map_codes.insert(std::make_pair<int, std::string>(257, " :<admin info>"));
	map_codes.insert(std::make_pair<int, std::string>(258, " :<admin info>"));
	map_codes.insert(std::make_pair<int, std::string>(259, " :<admin info>"));
}


// DEBUG
// выводит список существующих на сервере каналов и пользователей на них

void IRC::print_channels() const {
	for (channel_map::const_iterator it = _channels.begin(); it != _channels.end(); it++) {
		std::cout << "\tChannel: " << it->first << std::endl;
		it->second.print_users();
	}
}

