#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: QUIT
** ====================================================================
** Параметры: [<Quit message>]
** ====================================================================
** Сессия клиента заканчивается с QUIT-сообщением. Сервер должен закрыть 
** соединение с клиентом, когда увидит посланное сообщение. 
** При нетсплите (разрыве соединения между двумя серверами), сообщение 
** QUIT содержит в себе имена двух серверов, разделенные пробелами. 
** Первое имя это сервер, который соединяется, второе имя сервера, который 
** отсоединился. 
** Если, по какой-либо причине, соединение клиента закрылось без введения 
** клиентом команды QUIT (например, обрыв связи), сервер потребует запаса 
** в quit-сообщениях с некоторой сортировкой сообщения, в поисках причины 
** разрыва. 
** =====================================================================
*/

int Command::cmd_quit(IRC& irc, int fd)
{
    vector<User*>& vec_users = irc.get_users();
    vector<Server*>& vec_servers = irc.get_servers();
    channel_map ch_map = irc.get_channels();
    int pos_user;

    if ((pos_user = irc.find_fd(vec_users, fd)) != -1) // если от клиента
    {
        //удаляем юзера со всех каналов текущего сервера
        channel_map::iterator it_begin = ch_map.begin();
        channel_map::iterator it_end = ch_map.end();
        while (it_begin != it_end)
        {
            user_map temp_map = it_begin->second.get_users();
            user_map::iterator it_user = temp_map.find(vec_users[pos_user]);
            if (it_user != temp_map.end())
            {
                temp_map.erase(it_user);
                if (temp_map.size() == 0) // если канал пуст удалить его из мапы
                {
                    channel_map::iterator temp = ++it_begin;
                    ch_map.erase(it_begin);
                    it_begin = temp;
                    continue;
                }
                else //иначе отправить всем пользователем канал сообщение о выходе юзера
                {
                    user_map::iterator it_begin = temp_map.begin();
                    user_map::iterator it_end = temp_map.end();
                    while (it_begin != it_end)
                    {
                        irc.push_cmd_queue(it_begin->first->getSocketFd(), ":" + vec_users[pos_user]->getNickname() + " QUIT :Client closed connection");
                        ++it_begin;
                    }
                }
            }
            // удаляем из бан списка
            vector<User*>::iterator it_ban_start = it_begin->second.getVecBanned().begin();
            vector<User*>::iterator it_ban_end = it_begin->second.getVecBanned().end();

            while (it_ban_start != it_ban_end)
            {
                if ((*it_ban_start)->getNickname() == vec_users[pos_user]->getNickname())
                {
                    it_begin->second.getVecBanned().erase(it_ban_start);
                    break;
                }
                ++it_ban_start;
            }
            ++it_begin;
        }
        
        // отправляем всем серверам информацию о том что клиент вышел
        irc.forward_to_servers(fd, "QUIT :Client closed connection", true);
        
        //закрываем соединение
        irc.close_connect(fd, 0);
    }
    else if (irc.find_fd(vec_servers, fd) != -1) //если от сервера
    {
        std::cout << "Здесь будет логика сервера\n";
    }
    return (0);
}
