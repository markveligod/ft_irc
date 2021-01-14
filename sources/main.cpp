#include "./main.hpp"
#include "Class.IRC.hpp"

int g_exit;
std::map<int, std::string> map_codes;

void terminate_server(int signal)
{
	if (signal == SIGINT)
	{
		g_exit = 1;
	}
}

int main(int ac, char **av)
{
	std::vector<std::string> network;
	g_exit = 0;

	if (ac != 3 && ac != 4)
		utils::exit_error(ERR_COUNT, "ARG: ./ircserv [host:port_network:password_network] <port> <password>");
	// if (!utils::check_av(ac, av))
	// 	utils::exit_error(ERR_CHECKAV, "Incorrect enter to argv!");

	if (ac == 4)
	{
		network = utils::split(av[1], ':');
		if (network.size() < 3)
			network.push_back(string());
	}
	else
	{
		network.push_back(LOCALHOST);
		network.push_back(DEF_PORT);
		network.push_back(DEF_PASS);
	}
	generate_map_codes();
	
	IRC server(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1], (ac == 4) ? av[3] : av[2]);
	IRC server_ssl(network[0], network[1], network[2], (ac == 4) ? av[2] : av[1]	, (ac == 4) ? av[3] : av[2]);
	signal(SIGINT, terminate_server);
	if (ac == 4)
		server.create_socket_network(network);

	server.init_ssl();
	server.init_ctx();

	server.create_socket_local();
	while (true)
	// for (int i = 0; i < 3; i++)
	{
		server.init_fd_select();
		// std::cout << "DEBUG: INIT DONE!\n";
		server.do_select();
		// std::cout << "DEBUG: DO_SELECT DONE!\n";
		server.check_fd_select();
		// std::cout << "DEBUG: CHECK_FD_SELECT DONE!\n";
		if (g_exit == 1 && server.get_servers().size() == 0 && server.is_empty_queue() == true)
			exit(EXIT_SUCCESS);
		// std::cout << "DEBUG: G_EXIT: " << g_exit << " SERVER SIZE: " << server.get_servers().size() << " IS_EMPTY: " << server.is_empty_queue() << std::endl;
		if (g_exit == 1 && server.get_servers().size() != 0)
		{
			std::cout << "DEBUG: CTRL + C DONE!\n";
			for (size_t i = 0; i < server.get_servers().size(); i++)
			{
				server.push_cmd_queue(server.get_servers()[i]->getSocketFd(), "SQUIT " + server.get_servers()[i]->getServerName() + " :terminate\r\n");
			}
			server.get_servers().clear();
		}
	}
	return (0);
}

void generate_map_codes()
{
	map_codes.insert(std::make_pair<int, std::string>(401, "<nickname> :No such nick/channel"));
	map_codes.insert(std::make_pair<int, std::string>(402, "<server name> :No such server"));
	map_codes.insert(std::make_pair<int, std::string>(403, "<channel name> :No such channel"));
	map_codes.insert(std::make_pair<int, std::string>(404, "<channel name> :Cannot send to channel"));
	map_codes.insert(std::make_pair<int, std::string>(405, "<channel name> :You have joined too many channels"));
	map_codes.insert(std::make_pair<int, std::string>(406, "<nickname> :There was no such nickname"));
	map_codes.insert(std::make_pair<int, std::string>(407,  "<target> :Duplicate recipients. No message delivered"));
	map_codes.insert(std::make_pair<int, std::string>(409, ":No origin specified"));
	map_codes.insert(std::make_pair<int, std::string>(411, ":No recipient given (<command>)"));
	map_codes.insert(std::make_pair<int, std::string>(412, ":No text to send"));
	map_codes.insert(std::make_pair<int, std::string>(413, "<mask> :No toplevel domain specified"));
	map_codes.insert(std::make_pair<int, std::string>(414, "<mask> :Wildcard in toplevel domain"));
	map_codes.insert(std::make_pair<int, std::string>(421,  "<command> :Unknown command" ));
	map_codes.insert(std::make_pair<int, std::string>(422, ":MOTD File is missing"));
	map_codes.insert(std::make_pair<int, std::string>(423, "<server> :No administrative info available" ));
	map_codes.insert(std::make_pair<int, std::string>(424, ":File error doing <file op> on <file>"));
	map_codes.insert(std::make_pair<int, std::string>(431, ":No nickname given"));
	map_codes.insert(std::make_pair<int, std::string>(432, "<nick> :Erroneus nickname"));
	map_codes.insert(std::make_pair<int, std::string>(433, "<nick> :Nickname is already in use"));
	map_codes.insert(std::make_pair<int, std::string>(436, "<nick> :Nickname collision KILL"));
	map_codes.insert(std::make_pair<int, std::string>(441, "<nick> <channel> :They aren't on that channel"));
	map_codes.insert(std::make_pair<int, std::string>(442, "<channel> :You're not on that channel"));
	map_codes.insert(std::make_pair<int, std::string>(443, "<user> <channel> :is already on channel"));
	map_codes.insert(std::make_pair<int, std::string>(444, "<user> :User not logged in"));
	map_codes.insert(std::make_pair<int, std::string>(445, ":SUMMON has been disabled"));
	map_codes.insert(std::make_pair<int, std::string>(446, ":USERS has been disabled"));
	map_codes.insert(std::make_pair<int, std::string>(451, ":You have not registered"));
	map_codes.insert(std::make_pair<int, std::string>(461, "<command> :Not enough parameters"));
	map_codes.insert(std::make_pair<int, std::string>(462, ":You may not reregister"));
	map_codes.insert(std::make_pair<int, std::string>(463, ":Your host isn't among the privileged"));
	map_codes.insert(std::make_pair<int, std::string>(464, ":Password incorrect"));
	map_codes.insert(std::make_pair<int, std::string>(465, ":You are banned from this server"));
	map_codes.insert(std::make_pair<int, std::string>(467, "<channel> :Channel key already set"));
	map_codes.insert(std::make_pair<int, std::string>(471, "<channel> :Cannot join channel (+l)"));
	map_codes.insert(std::make_pair<int, std::string>(472, "<char> :is unknown mode char to me"));
	map_codes.insert(std::make_pair<int, std::string>(473, "<channel> :Cannot join channel (+i)"));
	map_codes.insert(std::make_pair<int, std::string>(474, "<channel> :Cannot join channel (+b)"));
	map_codes.insert(std::make_pair<int, std::string>(475, "<channel> :Cannot join channel (+k)"));
	map_codes.insert(std::make_pair<int, std::string>(481, ":Permission Denied- You're not an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(483, ":You cant kill a server!"));
	map_codes.insert(std::make_pair<int, std::string>(491, ":No O-lines for your host"));
	map_codes.insert(std::make_pair<int, std::string>(501, ":Unknown MODE flag"));
	map_codes.insert(std::make_pair<int, std::string>(502, ":Cant change mode for other users"));
	map_codes.insert(std::make_pair<int, std::string>(300, "None"));
	map_codes.insert(std::make_pair<int, std::string>(302, ":[<reply>{<space><reply>}]"));
	map_codes.insert(std::make_pair<int, std::string>(303, ":[<nick> {<space><nick>}]"));
	map_codes.insert(std::make_pair<int, std::string>(301, "<nick> :<away message>"));
	map_codes.insert(std::make_pair<int, std::string>(305, ":You are no longer marked as being away"));
	map_codes.insert(std::make_pair<int, std::string>(306, ":You have been marked as being away"));
	map_codes.insert(std::make_pair<int, std::string>(311, "<nick> <user> <host> * :<real name>"));
	map_codes.insert(std::make_pair<int, std::string>(312, "<nick> <server> :<server info>"));
	map_codes.insert(std::make_pair<int, std::string>(313, "<nick> :is an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(317, "<nick> <integer> :seconds idle"));
	map_codes.insert(std::make_pair<int, std::string>(318, "<nick> :End of /WHOIS list"));
	map_codes.insert(std::make_pair<int, std::string>(319, "<nick> :{[@|+]<channel><space>}"));
	map_codes.insert(std::make_pair<int, std::string>(314, "<nick> <user> <host> * :<real name>"));
	map_codes.insert(std::make_pair<int, std::string>(369, "<nick> :End of WHOWAS"));
	map_codes.insert(std::make_pair<int, std::string>(321, "Channel :Users  Name"));
	map_codes.insert(std::make_pair<int, std::string>(322, "<channel> <# visible> :<topic>"));
	map_codes.insert(std::make_pair<int, std::string>(323, ":End of /LIST"));
	map_codes.insert(std::make_pair<int, std::string>(324, "<channel> <mode> <mode params>"));
	map_codes.insert(std::make_pair<int, std::string>(331, "<channel> :No topic is set"));
	map_codes.insert(std::make_pair<int, std::string>(332, "<channel> :<topic>"));
	map_codes.insert(std::make_pair<int, std::string>(341, "<channel> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(342, "<user> :Summoning user to IRC"));
	map_codes.insert(std::make_pair<int, std::string>(351, "<version>.<debuglevel> <server> :<comments>"));
	map_codes.insert(std::make_pair<int, std::string>(352, "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"));
	map_codes.insert(std::make_pair<int, std::string>(315, "<name> :End of /WHO list"));
	map_codes.insert(std::make_pair<int, std::string>(353, "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"));
	map_codes.insert(std::make_pair<int, std::string>(366, "<channel> :End of /NAMES list"));
	map_codes.insert(std::make_pair<int, std::string>(364, "<mask> <server> :<hopcount> <server info>"));
	map_codes.insert(std::make_pair<int, std::string>(365, "<mask> :End of /LINKS list"));
	map_codes.insert(std::make_pair<int, std::string>(367, "<channel> <banid>"));
	map_codes.insert(std::make_pair<int, std::string>(368, "<channel> :End of channel ban list"));
	map_codes.insert(std::make_pair<int, std::string>(371, ":<string>"));
	map_codes.insert(std::make_pair<int, std::string>(374, ":End of /INFO list"));
	map_codes.insert(std::make_pair<int, std::string>(375, ":- <server> Message of the day - "));
	map_codes.insert(std::make_pair<int, std::string>(372, ":- <text>"));
	map_codes.insert(std::make_pair<int, std::string>(381, ":You are now an IRC operator"));
	map_codes.insert(std::make_pair<int, std::string>(382, "<config file> :Rehashing"));
	map_codes.insert(std::make_pair<int, std::string>(391, "<server> :<string showing server's local time>"));
	map_codes.insert(std::make_pair<int, std::string>(392, ":UserID   Terminal  Host"));
	map_codes.insert(std::make_pair<int, std::string>(393, ":%-8s %-9s %-8s"));
	map_codes.insert(std::make_pair<int, std::string>(394, ":End of users"));
	map_codes.insert(std::make_pair<int, std::string>(395, ":Nobody logged in"));
	map_codes.insert(std::make_pair<int, std::string>(200, "Link <version & debug level> <destination> <next server>"));
	map_codes.insert(std::make_pair<int, std::string>(201, "Try. <class> <server>"));
	map_codes.insert(std::make_pair<int, std::string>(202, "H.S. <class> <server>"));
	map_codes.insert(std::make_pair<int, std::string>(203, "???? <class> [<client IP address in dot form>]"));
	map_codes.insert(std::make_pair<int, std::string>(204, "Oper <class> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(205, "User <class> <nick>"));
	map_codes.insert(std::make_pair<int, std::string>(206, "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"));
	map_codes.insert(std::make_pair<int, std::string>(208, "<newtype> 0 <client name>"));
	map_codes.insert(std::make_pair<int, std::string>(261, "File <logfile> <debug level>"));
	map_codes.insert(std::make_pair<int, std::string>(211, "<linkname> <sendq> <sent messages> <sent bytes> <received messages> <received bytes> <time open>" ));
	map_codes.insert(std::make_pair<int, std::string>(212, "<command> <count>"));
	map_codes.insert(std::make_pair<int, std::string>(213, "C <host> * <name> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(214, "N <host> * <name> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(215, "I <host> * <host> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(216, "K <host> * <username> <port> <class>"));
	map_codes.insert(std::make_pair<int, std::string>(218, "Y <class> <ping frequency> <connect frequency> <max sendq>" ));
	map_codes.insert(std::make_pair<int, std::string>(219, "<stats letter> :End of /STATS report"));
	map_codes.insert(std::make_pair<int, std::string>(241, "L <hostmask> * <servername> <maxdepth>"));
	map_codes.insert(std::make_pair<int, std::string>(242, ":Server Up %d days %d:%02d:%02d"));
	map_codes.insert(std::make_pair<int, std::string>(243, "O <hostmask> * <name>"));
	map_codes.insert(std::make_pair<int, std::string>(244, "H <hostmask> * <servername>"));
	map_codes.insert(std::make_pair<int, std::string>(221, "<user mode string>"));
	map_codes.insert(std::make_pair<int, std::string>(251,  ":There are <integer> users and <integer> invisible on <integer> servers"));
	map_codes.insert(std::make_pair<int, std::string>(252, "<integer> :operator(s) online"));
	map_codes.insert(std::make_pair<int, std::string>(253, "<integer> :unknown connection(s)"));
	map_codes.insert(std::make_pair<int, std::string>(254, "<integer> :channels formed"));
	map_codes.insert(std::make_pair<int, std::string>(255, ":I have <integer> clients and <integer> servers"));
	map_codes.insert(std::make_pair<int, std::string>(256, "<server> :Administrative info"));
	map_codes.insert(std::make_pair<int, std::string>(257, ":<admin info>"));
	map_codes.insert(std::make_pair<int, std::string>(258, ":<admin info>"));
	map_codes.insert(std::make_pair<int, std::string>(259, ":<admin info>"));
}
