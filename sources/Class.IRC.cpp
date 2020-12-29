#include "Class.IRC.hpp"

# define FD_FREE 0
# define FD_CLIENT 1
# define FD_SERVER 2
# define FD_CLIENT_SSL 3
# define FD_SERVER_SSL 4

# define CERTIFICATE "cert/cert.pem"
# define PRIVATE_KEY "cert/key.pem"

/*
** ----------------------------------------------------------
** Constructors
** ----------------------------------------------------------
*/

IRC::IRC() {}

IRC::IRC(std::string network_ip,
		 std::string network_port,
		 std::string network_pass,
		 std::string localhost_port,
		 std::string localhost_pass)
{
	_network_ip = network_ip;
	_network_port = std::atoi(network_port.c_str());
	_network_pass = network_pass;
	_localhost_pass = localhost_pass;
	int port = std::atoi(localhost_port.c_str());
	_localhost = Socket(LOCALHOST, port);
	_localhost_ssl = Socket(LOCALHOST, port + 1);
	Utils::print_line("Constructor IRC done!");
	Utils::print_line("Socket local done!");
}

/*
** ----------------------------------------------------------
** Creating sockets
** ----------------------------------------------------------
*/

void IRC::create_socket_local()
{
	// local socket created on IRC conctructor

	_array_fd_select[_localhost._socket()] = FD_SERVER;
	Utils::print_line("Socket local FD done!");

	_array_fd_select[_localhost_ssl._socket()] = FD_SERVER_SSL;
	Utils::print_line("Socket ssl local FD done!");

	_localhost._bind();
	Utils::print_line("Socket local bind done!");

	_localhost_ssl._bind();
	Utils::print_line("Socket ssl local bind done!");

	_localhost._listen();
	Utils::print_line("Socket local listen...");

	_localhost_ssl._listen();
	Utils::print_line("Socket ssl local listen...");
}

void IRC::create_socket_network()
{
	_network = Socket(_network_ip.c_str(), _network_port);
	Utils::print_line("Socket network done!");
	int fd = _network._socket();
	Utils::print_line("Socket network FD done!");
	_network._connect();
	Utils::print_line("Socket network connection!");

	_array_fd_select[fd] = FD_CLIENT;
	std::string pass = "PASS " + _network_pass + "\r\n";
	send(fd, pass.c_str(), pass.size(), 0);
	// отправить список пользователей и каналов
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

void IRC::do_command(Command *command, int socket_fd)
{
	std::string cmd_name[3] =	{"NICK",
							   	 "PASS",
							  	 "USER"};
	doCommand	cmd_func[3] = 	{&Command::cmd_nick,
							   	 &Command::cmd_pass,
							   	 &Command::cmd_user};

	for (int i = 0; i < 3; i++)
		if (cmd_name[i] == command->getCommand())
		{
			(command->*(cmd_func[i]))(*this, socket_fd);
			return;
		}
	Utils::print_error(123, "Command not found");
}

/*
** ======================SELECT PART=========================
**
** ----------------------------------------------------------
** init_fd_select - добавление сокета в сет файловых
**					дескрипотов для селекта
** ----------------------------------------------------------
*/

void IRC::init_fd_select()
{
	FD_ZERO(&_fd_set_sockets);
	for (std::map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end(); it++)
		FD_SET(it->first, &_fd_set_sockets);
}

/*
** ----------------------------------------------------------
** do_select - селект!
** ----------------------------------------------------------
*/

void IRC::do_select()
{
	if ((_select_res = select(FD_SETSIZE, &_fd_set_sockets, NULL, NULL, NULL)) < 0)
		Utils::print_error(ERR_SELECT, "SELECT");
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

void IRC::check_fd_select()
{
	for (std::map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end() && _select_res > 0; it++)
	{
		if (FD_ISSET(it->first, &_fd_set_sockets))
		{
			if (it->second == FD_CLIENT || it->second == FD_CLIENT_SSL)
			{
				std::cout << "Client#" << it->first << ": ";
				
				char buffer[512 + 1];
				int n = _recv(it->second, it->first, buffer, 512, 0);
				
				if (n < 1)
				{
					it = _array_fd_select.begin();
					continue;
				}
				buffer[n] = '\0';
				//получаем распарсенный вектор команд если нашли \r\n
				std::vector<std::string> buffer_cmd = this->check_buffer(it->first, buffer);

				for (size_t i = 0; i < buffer_cmd.size(); i++)
				{
					Command mess(buffer_cmd[i]);

					std::cout << buffer_cmd[i] << std::endl;

					// передаем в исполнение команды сообщение и сокет, из которого пришло сообщение
					this->do_command(&mess, it->first);
					_send(it->second, it->first, buffer_cmd[i].c_str(), strlen(buffer_cmd[i].c_str()), 0);
				}
				bzero(buffer, 512);
			}
			//Раздел для приемки сообщений из селекта
			if (it->second == FD_SERVER || it->second == FD_SERVER_SSL)
			{
				// _accept() возвращает fd клиента, который мы добавляем в map
				// в качестве ключа, в качестве значения добавляем FD_CLIENT(_SSL)
				char response[] = "Accepted\n";

				int client_socket = (it->second == FD_SERVER)
									? _localhost._accept()
									: _localhost_ssl._accept();

				_array_fd_select[client_socket] = (it->second == FD_SERVER)
																? FD_CLIENT
																: FD_CLIENT_SSL;
				if (it->second == FD_SERVER_SSL)
					ssl_connection(client_socket);
											
				std::cout << "New client#" << client_socket << " joined server\n";
				_send(it->second, client_socket, response, 10, 0);
				this->_clients.push_back(new Client(client_socket));
			}
			_select_res--;
		}
	}
}

int IRC::_send(int connection_type, int fd, const char *response, size_t size, int flags)
{
	int n;

	if (connection_type == FD_CLIENT || connection_type == FD_SERVER)
		n = send(fd, reinterpret_cast<const void *>(response), size, flags);
	else
		n = SSL_write(_ssl, reinterpret_cast<const void *>(response), size);
	
	if (n < 0)
		std::cout << "message sending failed\n";
	
	return n;
}

int IRC::_recv(int connection_type, int fd, char *response, size_t size, int flags)
{
	int n;

	if (connection_type == FD_CLIENT || connection_type == FD_SERVER)
		n = recv(fd, reinterpret_cast<void *>(response), size, flags);
	else
		n = SSL_read(_ssl, reinterpret_cast<void *>(response), size);
	
	if (n == 0 || n < 0)
	{
		this->delete_client(fd);
		this->delete_user(fd);

		std::cout << (n == 0 ? "connection closed\n" : "message receiving failed\n");
		_array_fd_select.erase(fd);
	}
	return n;
}

/*
** ========================SSL PART==========================
**
** ----------------------------------------------------------
** init_ctx - инициализация конфигурации для SSL
** ----------------------------------------------------------
*/

void IRC::init_ssl()
{
	SSL_library_init();					/*load encryption and hash algo's in ssl*/
	OpenSSL_add_all_algorithms();		/* load & register all cryptos, etc. */
	ERR_load_crypto_strings();		
	SSL_load_error_strings();			/* load all error messages */
}

void IRC::init_ctx()
{
	const SSL_METHOD *method;

	// if (server)
		method = TLS_server_method();	/* create new server-method instance */
	// if (client)
	// 	method = TLS_client_method();
	if (!(_ctx = SSL_CTX_new(method)))	/* create new context from method */
		Utils::print_error(1, "SSL: SSL_CTX_new failed\n");

	if (SSL_CTX_use_certificate_file(_ctx, CERTIFICATE, SSL_FILETYPE_PEM) <= 0)
		Utils::print_error(1, "SSL: SSL_CTX_use_certificate_file failed\n");
	if (SSL_CTX_use_PrivateKey_file(_ctx, PRIVATE_KEY, SSL_FILETYPE_PEM) <= 0)
		Utils::print_error(1, "SSL: SSL_CTX_use_PrivateKey_file SSL failed\n");
	if (!SSL_CTX_check_private_key(_ctx))
		Utils::print_error(1, "SSL: SSL_CTX_check_private_key failed\n");
}

SSL *IRC::ssl_connection(int fd)
{
	_ssl = SSL_new(_ctx);

	if (SSL_set_fd(_ssl, fd) <= 0)
		Utils::print_error(1, "SSL: SSL_set_fd failed\n");

	if (SSL_accept(_ssl) < 0)		/* do SSL-protocol accept */
		Utils::print_error(1, "SSL: SSL_accept failed\n");

	return _ssl;
}

/*
** ===========================UTILS PART=============================
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

void	IRC::delete_user(int fd)
{
	int i;
	if ((i = IRC::find_fd(&this->_users, fd)) > -1)
	{
		User *out_user = (*(this->_users.begin() + i));
		this->_users.erase(this->_users.begin() + i);
		delete out_user;
	}
}

/*
** ВАЖНО
** когда к нам добавятся юзеры из других серверов, так удалять будет нельзя!
** нужно будет смотреть на префиксы, а не только на fd
*/

void	IRC::delete_client(int fd)
{
	int i;
	if ((i = IRC::find_fd(&this->_clients, fd)) > -1)
	{
		Client *out_client = (*(this->_clients.begin() + i));
		this->_clients.erase(this->_clients.begin() + i);
		delete out_client;
	}
}

/*
** ----------------------------------------------------------
** check_buffer - возвращает расспарсенный вектор буфера с соединенными командами
** ----------------------------------------------------------
*/

std::vector<std::string> IRC::check_buffer(int fd, const char *buffer)
{
	Client *temp_ptr_client = this->_clients[IRC::find_fd(&this->_clients, fd)];
	std::vector<std::string> temp_vec;

	std::string temp = temp_ptr_client->getBuffer();
	temp_ptr_client->setBuffer(temp + static_cast<std::string>(buffer));
	
	while (temp_ptr_client->find_line_break())
	{
		std::string temp_str = temp_ptr_client->get_line_break();
		if (temp_str == "CAP LS")
			continue ;
		temp_vec.push_back(temp_str);
		std::cout << "TEMP_VEC: " << temp_vec.back() << std::endl;
	}
	return (temp_vec);
}

/*
** ----------------------------------------------------------
** Channel
** ----------------------------------------------------------
*/

void IRC::join_channel(string channel_name, string nickname)
{
	if (!_channels.count(channel_name))
	{
		_channels.insert(make_pair(channel_name, Channel(channel_name, nickname, *this)));
		// отправить всем серверам, что создан канал
	}
	else
	{
		// if (_channels[channel_name].is_banned(nickname)) TODO
		// 	// отправить пользователю, что он забанен и не может подключиться

		int index = find_nickname(&_users, nickname);
		if (index >= 0)
		{
			map<string, Channel>::iterator it = _channels.find(channel_name);
			it->second.add_user(_users[index]);
		}
	}
	
}

User *					IRC::get_user(string nickname)
{
	int index = find_nickname(&_users, nickname);

	return (index >= 0) ? _users[index] : NULL;
}
vector<User *> &		IRC::get_users() {return (this->_users);}
vector<Client *> &		IRC::get_clients() {return (this->_clients);}
vector<Server *> &		IRC::get_servers() {return (this->_servers);}
map<string, Channel> &	IRC::get_channels() {return (this->_channels);}
string const &			IRC::get_localhost_pass() const {return (this->_localhost_pass);}
