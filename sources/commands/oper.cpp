#include "Class.Command.hpp"
#include "Class.IRC.hpp"

/*
** ====================================================================
** Команда: OPER 
** ====================================================================
** Параметры: <user> <password> 
** Сообщение OPER используется нормальным пользователем для взятия 
** операторских привилегий. Комбинация <user> и <password> используется 
** для идентификации пользователя, запрашивающего права IRC-оператора. 
** Если клиент послал команду OPER с корректным паролем для текущего 
** пользователя, сервер информамирует сеть о новом операторе, используя 
** "MODE +o" для никнейма клиента. 
** Сообщение OPER только для клиент-сервер.
** Пример: 
** OPER foo bar 
**
** Попытка зарегистрироваться как оператору, используя имя пользователя "foo" и пароль "bar".
** =====================================================================
*/

int Command::cmd_oper(IRC& irc, int fd)
{
    std::vector<User *> & vec_users = irc.get_users();
    std::vector<Server *>& vec_servers = irc.get_servers();
    int pos;

    if (!this->check_args_number(2))
		return (ERR_NEEDMOREPARAMS);
    if ((pos = irc.find_fd(vec_users, fd)) >= 0)
        return (ERR_ALREADYREGISTRED);
    if (this->arguments[0] == irc.get_operator_user() && this->arguments[1] == irc.get_operator_pass())
        irc.push_cmd_queue(vec_servers[irc.find_fd(vec_servers, irc.get_socket().get_fd())]->getFdSocket(), "MODE " + vec_users[pos]->getNickname() + " +o\r\n");
    else
        return (ERR_PASSWDMISMATCH);
    return (0);
}