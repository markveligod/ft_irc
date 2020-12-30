
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
        std::string _server_name;
        int _hopcount;
        std::string _info;
        std::vector<User *> _users;
        //std::vector<Channel *> _channels

        Server();
        Server(const Server & other);
        Server &operator=(const Server & other);
    public:
        Server(const int & fd, const std::string & servername, const int & hopcount, const std::string & info);
        ~Server();

        const int & getFdSocket() const;
        void addUser(User *new_user);
};

