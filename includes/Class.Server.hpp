
/*
**==========================
** Class Server -
** Обмен информацией о состоянии соединения сервер - сервер:
**  - все знают другие сервера; 
**  - все знают пользовательскую информацию; 
**  - все знают информацию каналов.
**==========================
*/

#include "main.hpp"
#include "Class.User.hpp"

class Server
{
    private:
        int _fd_socket;
        std::vector<User *> _users;
        //std::vector<Channel *> _channels

        Server();
        Server(const Server & other);
        Server &operator=(const Server & other);
    public:
        Server(const int & fd);
        ~Server();

        const int & getFdSocket() const;
        void addUser(User *new_user);
};

