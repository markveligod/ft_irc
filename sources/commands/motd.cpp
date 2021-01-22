#include "main.hpp"
#include "Class.Command.hpp"

/*
** =========================================================================
** Команда: MOTD 
** =========================================================================
** Параметры: [ <target> ]
** =========================================================================
** The MOTD command is used to get the "Message Of The Day" of the given
** server, or current server if <target> is omitted.
*/

void Command::
cmd_motd(IRC& irc, int fd)
{
    vector<User*>& users 		= irc.get_users();
    int pos_user                = irc.find_fd(users, fd);

    if (pos_user != -1 && this->_prefix.size() == 0) //если обратился клиент
    {
        std::cout << "\nDEBUG: раздел если обратился клиент\n";
        if (this->_arguments.size() > 1) //много параметров
        {
            irc.push_cmd_queue(fd, irc.response(422, users[pos_user]->getName(), "422", ":MOTD File is missing"));
            return ;
        }
        else if (this->_arguments.size() == 1) //если указан параметр target шлем по серверу
        {
            vector<Server *>& servers = irc.get_servers();
            int server_el_name = irc.find_name(servers, this->_arguments[0]);

            if (server_el_name == -1)
            {
                irc.push_cmd_queue(fd, irc.response(300, "None", "300", ":error find server"));
                return ;
            }
            irc.push_cmd_queue(servers[server_el_name]->getSocketFd(), ":" + users[pos_user]->getName() + " " + this->_command + "\r\n");
        }
        else // если не указан шлем по fd клиента
        {
            irc.push_cmd_queue(fd, irc.response(375, users[pos_user]->getNickname(), "375", ":- " + irc.get_server_name() + " Message of the day - "));

		    vector<string> temp_motd = irc.motd_generate();
		    for (size_t i = 0; i < temp_motd.size(); i++)
		    {
		    	irc.push_cmd_queue(fd, irc.response(372, users[pos_user]->getNickname(), "372", temp_motd[i]));
		    }

		    irc.push_cmd_queue(fd, irc.response(376, users[pos_user]->getNickname(), "376", ":End of MOTD command"));
        }
    }
    else // если обратился сервер
    {
        std::cout << "\nDEBUG: раздел если обратился сервер\n";
        if (this->_prefix.size() != 0)
        {
            int pos_user_prefix = irc.find_name(users, this->_prefix);
            
            if (pos_user_prefix == -1)
            {
                irc.push_cmd_queue(fd, irc.response(300, users[pos_user]->getName(), "300", ":None user"));
                return ;
            }
            
            irc.push_cmd_queue(users[pos_user_prefix]->getSocketFd(), irc.response(375, users[pos_user_prefix]->getNickname(), "375", ":- " + irc.get_server_name() + " Message of the day - "));

		    vector<string> temp_motd = irc.motd_generate();
		    for (size_t i = 0; i < temp_motd.size(); i++)
		    {
		    	irc.push_cmd_queue(users[pos_user_prefix]->getSocketFd(), irc.response(372, users[pos_user_prefix]->getNickname(), "372", temp_motd[i]));
		    }

		    irc.push_cmd_queue(users[pos_user_prefix]->getSocketFd(), irc.response(376, users[pos_user_prefix]->getNickname(), "376", ":End of MOTD command"));
        }
        else
        {
            irc.push_cmd_queue(fd, irc.response(300, users[pos_user]->getName(), "300", ":None prefix"));
            return ;
        }
    }
}
