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
	_network._socket();
	Utils::print_line("Socket network FD done!");
	_network._connect();
	Utils::print_line("Socket network connection!");
}

/*
** ---------------------------------------------------------------------------------------------
** do_command 	- выполняет команду, которая пришла в сообщении от клиента
** 				  (принимает на вход сообшение и дескриптор сокета, от которого оно пришло)
** суть работы -  создает массив имен команд, ссылок на соответствующие функции,
**				  и два массива аргументов для этих функций;
**				  Проходится в цикле по всем именам команд и запускает соответствующую функцию
**				  (метод класса message)
** ---------------------------------------------------------------------------------------------
*/

typedef void (Message::*doCommand)(void *, void *);

void IRC::do_command(Message *message, int socket_fd)
{
	std::string cmd_name[3] =	{"NICK",
							   	 "PASS",
							  	 "USER"};
	doCommand	cmd_func[3] = 	{&Message::cmd_nick,
							   	 &Message::cmd_pass,
							   	 &Message::cmd_user};
	void 		*cmd_var[3] = 	{(void *)&this->_clients,
							   	 (void *)&this->_clients,
							   	 (void *)&this->_clients};
	void 		*cmd_var2[3] =	{(void *)&socket_fd,
								 (void *)&socket_fd,
							 	 (void *)&this->_users};

	for (int i = 0; i < 3; i++)
		if (cmd_name[i] == message->getCommand())
		{
			(message->*(cmd_func[i]))(cmd_var[i], cmd_var2[i]);
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
			if (it->second == FD_CLIENT)
			{
				char buffer[512 + 1];
				int n = 0;
				std::cout << "Client#" << it->first << ": ";
				if (((n = recv(it->first, buffer, 512, 0))) == 0)
				{
					std::cout << "connection closed\n";
					_array_fd_select.erase(it->first);
				}
				else
				{
					Message mess;

					buffer[n] = '\0';
					std::cout << buffer << std::endl;
					mess.pars_str(buffer);

					// передаем в исполнение команды сообщение и сокет, из которого пришло сообщение
					this->do_command(&mess, it->first);
					_network._send(buffer);
				}
			}
			if (it->second == FD_CLIENT_SSL)
			{
				char buffer[512 + 1];
				int n = 0;
				std::cout << "Client ssl#" << it->first << ": ";
				if (((n = SSL_read(_ssl, buffer, 512))) <= 0)
				{
					std::cout << "ssl connection closed\n";
					_array_fd_select.erase(it->first);
				}
				else
				{
					Message mess;

					buffer[n] = '\0';
					std::cout << buffer << std::endl;
					mess.pars_str(buffer);

					// передаем в исполнение команды сообщение и сокет, из которого пришло сообщение
					this->do_command(&mess, it->first);
					_network._send(buffer);
				}
			}
			//Раздел для приемки сообщений из селекта
			if (it->second == FD_SERVER)
			{
				// _accept() возвращает fd клиента, который мы добавляем в map
				// в качестве ключа, в качестве значения добавляем FD_CLIENT
				char response[] = "Accepted\n";

				int client_socket = _localhost._accept();
				_array_fd_select[client_socket] = FD_CLIENT;
				std::cout << "New client#" << client_socket << " joined server\n";
				
				send(client_socket, reinterpret_cast<void *>(response), 10, 0);

				Client *newclient = new Client(client_socket);
				this->_clients.push_back(newclient);
			}
			if (it->second == FD_SERVER_SSL)
			{
				char response[] = "Accepted\n";

				int client_socket = _localhost_ssl._accept();
				_array_fd_select[client_socket] = FD_CLIENT_SSL;
				std::cout << "New ssl client#" << client_socket << " joined server\n";
				
				_ssl = ssl_connection(client_socket);
				SSL_write(_ssl, reinterpret_cast<void *>(response), 10);

				Client *newclient = new Client(client_socket);
				this->_clients.push_back(newclient);
			}
			_select_res--;
		}
	}
}

/*
**==========================
** init_ctx - инициализация конфигурации для SSL
**==========================
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
	SSL *ssl = SSL_new(_ctx);

	if (SSL_set_fd(ssl, fd) <= 0)
		Utils::print_error(1, "SSL: SSL_set_fd failed\n");

	if (SSL_accept(ssl) < 0)		/* do SSL-protocol accept */
		Utils::print_error(1, "SSL: SSL_accept failed\n");

	return ssl;
}