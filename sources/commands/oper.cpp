#include "Class.Command.hpp"
#include "Class.IRC.hpp"
#include "utils.hpp"

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
    int pos;

    if (!this->check_args_number(2))
		return (ERR_NEEDMOREPARAMS);
    if ((pos = irc.find_fd(vec_users, fd)) == -1)
        return (ERR_ALREADYREGISTRED);
    if (this->arguments[0] == irc.get_operator_user() && this->arguments[1] == irc.get_operator_pass())
    {
        utils::print_line("cmd_oper is correct");
        this->command = "MODE";
        this->arguments.clear();
        this->arguments.push_back(vec_users[pos]->getNickname());
        this->arguments.push_back("+o");
        utils::print_line("Mode run!");
        //this->cmd_mode(irc, fd);
    }
    else
        return (ERR_PASSWDMISMATCH);
    return (0);
}