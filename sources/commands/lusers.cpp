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
    vector<Server*>& servers    = irc.get_servers();
    int count = 0;
    bool isUser;
    bool isServer;

    for (size_t i = 0; i < clients.size(); i++)
    {
        isUser = false;
        isServer = false;
        for (size_t j = 0; j < users.size(); j++)
        {
            if (clients[i]->getSocketFd() == users[j]->getSocketFd())
                isUser = true;
        }
        for (size_t l = 0; l < servers.size(); l++)
        {
            if (clients[i]->getSocketFd() == servers[l]->getSocketFd())
                isServer = true;
        }
        if (!isUser && !isServer)
            count++;
    }
    return (count);
}

void Command::
cmd_lusers(IRC& irc, int fd)
{
    std::cout << "Command: " << this->_command << std::endl;
    std::cout << "Prefix: " << this->_prefix << std::endl;
    std::cout << "Arg:\n";
    for (size_t i = 0; i < this->_arguments.size(); i++)
    {
        std::cout << "Index #" << i << " " << this->_arguments[i] << std::endl;
    }

    vector<User*>& users 		= irc.get_users();
    vector<Server *>& servers   = irc.get_servers();

    int pos_user = irc.find_fd(users, fd);
    int pos_server = irc.find_fd(servers, fd);

    std::cout << "User: " << pos_user << std::endl;
    std::cout << "Server: " << pos_server << std::endl;

    if (pos_server == -1) // обратился клиент
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
        else if (this->_arguments.size() == 1 && this->_prefix.size() == 0) //есть параметр
        {
            std::cout << "\nDEBUG: с параметром\n";
            if (irc.get_server_name() == this->_arguments[0]) //если указали наш текущий сервер
            {
                irc.push_cmd_queue(fd, irc.response(251, users[pos_user]->getNickname(), "251 " + irc.get_server_name(), ":There are " + utils::int_to_str(users.size()) + " users and 0 services on " + utils::int_to_str(servers.size()) + " servers"));

                irc.push_cmd_queue(fd, irc.response(252, users[pos_user]->getNickname(), "252 " + irc.get_server_name(), utils::int_to_str(get_count_operators(irc)) + " :operator(s) online"));

                irc.push_cmd_queue(fd, irc.response(253, users[pos_user]->getNickname(), "253 " + irc.get_server_name(), utils::int_to_str(get_count_client(irc)) + " :unknown connection(s)"));

                irc.push_cmd_queue(fd, irc.response(254, users[pos_user]->getNickname(), "254 " + irc.get_server_name(), utils::int_to_str(irc.get_channels().size()) + " :channels formed"));

                irc.push_cmd_queue(fd, irc.response(255, users[pos_user]->getNickname(), "255 " + irc.get_server_name(), ":I have " + utils::int_to_str(get_count_my_users(irc)) + " clients and " + utils::int_to_str(get_count_my_servers(irc)) + " servers"));
                return ;
            }

            int pos_fullname_server = irc.find_name(servers, this->_arguments[0]);

            if (pos_fullname_server != -1) //полное имя сервера
            {
                irc.push_cmd_queue(servers[pos_fullname_server]->getSocketFd(), ":" + users[pos_user]->getName() + " LUSERS " + this->_arguments[0] + "\r\n");
            }
            else if (pos_fullname_server == -1 && this->_arguments[0].find("*") != std::string::npos) //маска серверов
            {
                size_t pos_start = this->_arguments[0].find("*");

                if (pos_start == (this->_arguments[0].size() - 1)) // звездочка в конце
                {
                    std::string temp(this->_arguments[0].begin(), --(this->_arguments[0].end()));
                    std::cout << "DEBUG: temp: " << temp << std::endl;
                    for (size_t i = 0; i < servers.size(); i++) //если попадают под маску другие сервера
                    {
                        if (servers[i]->getName().find(temp) != std::string::npos)
                            irc.push_cmd_queue(servers[i]->getSocketFd(), ":" + users[pos_user]->getName() + " LUSERS " + servers[i]->getName() + "\r\n");
                    }
                    if (irc.get_server_name().find(temp) != std::string::npos) //если под маску попадает наш текущий сервер
                    {
                        irc.push_cmd_queue(fd, irc.response(251, users[pos_user]->getNickname(), "251 " + irc.get_server_name(), ":There are " + utils::int_to_str(users.size()) + " users and 0 services on " + utils::int_to_str(servers.size()) + " servers"));

                        irc.push_cmd_queue(fd, irc.response(252, users[pos_user]->getNickname(), "252 " + irc.get_server_name(), utils::int_to_str(get_count_operators(irc)) + " :operator(s) online"));

                        irc.push_cmd_queue(fd, irc.response(253, users[pos_user]->getNickname(), "253 " + irc.get_server_name(), utils::int_to_str(get_count_client(irc)) + " :unknown connection(s)"));

                        irc.push_cmd_queue(fd, irc.response(254, users[pos_user]->getNickname(), "254 " + irc.get_server_name(), utils::int_to_str(irc.get_channels().size()) + " :channels formed"));

                        irc.push_cmd_queue(fd, irc.response(255, users[pos_user]->getNickname(), "255 " + irc.get_server_name(), ":I have " + utils::int_to_str(get_count_my_users(irc)) + " clients and " + utils::int_to_str(get_count_my_servers(irc)) + " servers"));
                    }
                }
                else
                {
                    std::cout << "\nDEBUG: (*_*) звездочка ты где ?\n";
                    irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
                    return ;   
                }
            }
            else //none
            {
                std::cout << "\nDEBUG: (*_*) как тебя распарсить ?\n";
                irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
                return ;    
            }
            
        }
        else //none
        {
            std::cout << "\nDEBUG: (*_*) шо такое ?\n";
            irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
            return ;
        }
    }
    else if (pos_server != -1) // обратился сервер
    {
        std::cout << "\nDEBUG: обратился сервер\n";
        if (this->_arguments.size() == 1 && this->_prefix.size() != 0) //ищем канал на соответсвие запроса
        {
            if (this->_arguments[0] == irc.get_server_name()) //совпадают шлем инфу
            {
                std::cout << "\nDEBUG: сервер найден\n";
                int user_pos_before = irc.find_name(users, this->_prefix);

                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS 251 " + irc.get_server_name() +  " :There are " + utils::int_to_str(users.size()) + " users and 0 services on " + utils::int_to_str(servers.size()) + " servers\r\n");

                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS 252 " + irc.get_server_name() + " " + utils::int_to_str(get_count_operators(irc)) + " :operator(s) online\r\n");
                
                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS 253 " + irc.get_server_name() + " " + utils::int_to_str(get_count_client(irc)) + " :unknown connection(s)\r\n");
                
                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS 254 " + irc.get_server_name() + " " + utils::int_to_str(irc.get_channels().size()) + " :channels formed\r\n");

                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS 255 " + irc.get_server_name() + " :I have " + utils::int_to_str(get_count_my_users(irc)) + " clients and " + utils::int_to_str(get_count_my_servers(irc)) + " servers\r\n");
            }
            else
            {
                int pos_next_server = irc.find_name(servers, this->_arguments[0]);
                irc.push_cmd_queue(servers[pos_next_server]->getSocketFd(), ":" + this->_prefix + " LUSERS " + this->_arguments[0] + "\r\n");
            }
        }
        else if (this->_arguments.size() > 1 && this->_prefix.size() != 0) //пересылаем обратно
        {
            std::cout << "\nDEBUG: пересылка обратно пользователю\n";
            int user_pos_before = irc.find_name(users, this->_prefix);

            std::stringstream temp;
            for (size_t i = 0; i < this->_arguments.size(); i++)
            {
                temp << this->_arguments[i] << " ";
            }
            temp << "\r\n";

            if (users[user_pos_before]->getHopcount() != 0)
                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), ":" + this->_prefix + " LUSERS " + temp.str());
            else
                irc.push_cmd_queue(users[user_pos_before]->getSocketFd(), temp.str());
            
        }
        else //none
        {
            std::cout << "\nDEBUG: (*_*) у тебя совесть есть ?\n";
            irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
            return ;    
        }
    }
    else //none
    {
        std::cout << "\nDEBUG: (*_*) кто ты ?\n";
        irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":NONE"));
        return ;
    }
    
}
