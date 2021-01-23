#include "main.hpp"
#include "Class.Command.hpp"

/*
** =========================================================================
** Команда: LUSERS
** =========================================================================
** Параметры: [ <mask> [ <target> ] ]
** =========================================================================
** The LUSERS command is used to get statistics about the size of the
** IRC network. If no parameter is given, the reply will be about the
** whole net. If a <mask> is specified, then the reply will only concern 
** the part of the network formed by the servers matching the
** mask. Finally, if the <target> parameter is specified, the request
** is forwarded to that server which will generate the reply.
*/

int get_count_operators(IRC& irc)
{
    vector<User*>& users 		= irc.get_users();
    int count = 0;

    for (size_t i = 0; i < users.size(); i++)
    {
        if (irc.is_server_operator(users[i]))
            count++;
    }
    return (count);
}

int get_count_my_users(IRC& irc)
{
    vector<User*>& users 		= irc.get_users();
    int count = 0;

    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i]->getHopcount() == 0)
            count++;
    }
    return (count);
}

int get_count_my_servers(IRC& irc)
{
    vector<Server *>& servers   = irc.get_servers();
    int count = 0;

    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i]->getHopcount() == 1)
            count++;
    }
    return (count);
}

int get_count_client(IRC& irc)
{
    vector<Client *>& clients   = irc.get_clients();
    vector<User*>& users 		= irc.get_users();
    int count = 0;

    for (size_t i = 0; i < clients.size(); i++)
    {
        for (size_t j = 0; j < users.size(); j++)
        {
            if (clients[i]->getName() != users[j]->getName() && clients[i]->getIsServer() == false)
                count++;
        }
    }
    return (count);
}

void Command::
cmd_lusers(IRC& irc, int fd)
{
    vector<User*>& users 		= irc.get_users();
    vector<Server *>& servers   = irc.get_servers();

    int pos_user = irc.find_fd(users, fd);
    int pos_server = irc.find_fd(servers, fd);

    if (pos_user != -1 && pos_server == -1) // обратился клиент
    {
        std::cout << "\nDEBUG: обратился клиент\n";
        if (this->_arguments.size() == 0 && this->_prefix.size() == 0) //нет параметров - выводим всю информацию о сети
        {
            std::cout << "\nDEBUG: без парметров\n";
            irc.push_cmd_queue(fd, irc.response(251, users[pos_user]->getNickname(), "251 " + irc.get_server_name(), ":There are " + utils::int_to_str(users.size()) + " users and 0 services on " + utils::int_to_str(servers.size()) + " servers"));

            irc.push_cmd_queue(fd, irc.response(252, users[pos_user]->getNickname(), "252 " + irc.get_server_name(), utils::int_to_str(get_count_operators(irc)) + " :operator(s) online"));

            irc.push_cmd_queue(fd, irc.response(253, users[pos_user]->getNickname(), "253 " + irc.get_server_name(), utils::int_to_str(get_count_client(irc)) + " :unknown connection(s)"));

            irc.push_cmd_queue(fd, irc.response(254, users[pos_user]->getNickname(), "254 " + irc.get_server_name(), utils::int_to_str(irc.get_channels().size()) + " :channels formed"));

            irc.push_cmd_queue(fd, irc.response(255, users[pos_user]->getNickname(), "255 " + irc.get_server_name(), ":I have " + utils::int_to_str(get_count_my_users(irc)) + " clients and " + utils::int_to_str(get_count_my_servers(irc)) + " servers"));

        }
    }
    else if (pos_user == -1 && pos_server != -1) // обратился сервер
    {
        std::cout << "\nDEBUG: обратился сервер\n";
    }
    else //none
    {
        std::cout << "\nDEBUG: (*_*)\n";
        irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
        return ;        
    }
    
}
