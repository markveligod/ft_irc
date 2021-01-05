#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

/*
** ====================================================================
** Команда: SQUIT
** ====================================================================
** Параметры: <server> <comment> 
** Сообщение SQUIT требуется для указания мертвых или вышедших серверов. 
** Если сервер желает оборвать соединение с другим сервером, он должен 
** послать сообщение SQUIT другому серверу, используя имя другого сервера 
** в качестве параметра server. 
** Эта команда так же доступна IRC-операторам для помощи в сохранеии сети 
** соединений IRC-серверов в порядке. IRC-операторы могут так же послать 
** SQUIT для удаленных серверных соединений. В этом случае, SQUID будет 
** парситься каждым сервером, находящимся между IRC-оператором и удаленным 
** сервером. 
** Параметр <comment> обеспечивается всеми операторами, которые запускают 
** SQUIT для удаленных серверов (которые не присоединены к серверу, 
** который хотят выключить), так что все операторы знают причины этого 
** действия. 
** Один из серверов, которые находятся на другой стороне соединения, будет 
** закрыт по требованию, высланным сообщением SQUID (ко всем другим 
** соединениям) дл остальных серверво, которые рассматриваются как линки. 
** Подобным образом, SQUIT может быть послана другим серверам, находящимся 
** в сети ради клиентов. В дополнение к этому, все члены канала, который 
** разбило сплитом, может послать SQUIT-сообщение. 
** Если соединение сервера закрыто преждевременно (т.е сервер на другом 
** конце соединения разорвал линк), сервер, который засек этот разрыв 
** соединения, информирует всю сеть о том, что соединение закрыто и 
** показывает поле <comment>, обьясняя причину рассоединения. 
** =====================================================================
*/

int Command::cmd_squit(IRC& irc, int fd)
{
    std::vector<Server*>& vec_servers = irc.get_servers();
    int pos;

    if (!this->check_args_number(2))
    {
        irc.push_cmd_queue(fd, irc.response_to_client(ERR_NEEDMOREPARAMS, fd, "ERR_NEEDMOREPARAMS", "Not enought parameters"));
		return (ERR_NEEDMOREPARAMS);
    }
    if ((pos = irc.find_fd(vec_servers, fd)) == -1)
    {
        irc.push_cmd_queue(fd, irc.response_to_client(ERR_ALREADYREGISTRED, fd, "ERR_ALREADYREGISTRED", "Dont found server"));
        return (ERR_ALREADYREGISTRED);
    }
    Server* out_server = vec_servers[pos];
    delete out_server;
    vec_servers.erase(vec_servers.begin() + pos);
    return (0);
}
