# :white_check_mark: ft_irc (project 2020-2021)
  
This test project is intended for a deeper study of the operation of sockets and their interactions on the example of an IRC chat/server.  
The test project is built via a Makefile and is run using two commands:  
`./ircserv [port] [pass]` if you need to start a single network  
`./ircserv [ip:port:pass] [port] [pass]` if you need to join an existing network  
More detailed information is contained in subject.txt  
📌If you have any questions or find bugs, write to the issues section.  
  
## <img src="https://github.com/markveligod/ft_irc/blob/main/img/chat.gif" width="32" height="32" > Description
IRC (Internet Relay Chat) - an application-level Protocol for exchanging messages in real-time.  
Designed primarily for group communication, it also allows you to communicate via private messages and exchange data, including files.  
IRC uses the TCP transport protocol and cryptographic TLS (optional).  
IRC began to gain particular popularity after Operation "Desert Storm" (1991), when messages from all over the world were collected in one place and broadcast on-line to IRC  
Due to the technical simplicity of implementation, the IRC protocol was used in the organization of botnets as a means of transmitting control commands to the computers participating in the botnet from the owner.  
### :earth_americas: IRC network
According to the protocol specifications, an IRC network is a group of servers connected to each other. The simplest network is a single server.  
The network should have the form of a connected tree, in which each server is the central node for the rest of the network.  
A client is anything that is connected to the server, except for other servers. There are two types of customers:  
- custom settings;  
- service stations.  
  
<img src="https://github.com/markveligod/ft_irc/blob/main/img/network.svg">
  
### Forwarding messages in the IRC network
IRC provides both group and private communication.  
There are several possibilities for group communication.  
A user can send a message to a list of users, and a list is sent to the server, the server selects individual users from it, and sends a copy of the message to each of them.  
More efficient is the use of channels. In this case, the message is sent directly to the server, and the server sends it to all users in the channel.  
In both group and private communication, messages are sent to clients via the shortest path and are visible only to the sender, recipient, and incoming servers.  
It is also possible to send a broadcast message. Client messages regarding changes in the network state (for example, channel mode or user status) must be sent to all servers that are part of the network. All messages originating from the server must also be sent to all other servers.  

### 📄 Run server
When you run the `make server` command, it compiles and runs on port 1080  
Our server implementation supports the following commands:  
* admin 
* away 
* connect 
* error
* info
* invite
* join
* kick
* kill
* links
* list
* lusers
* mode
* motd
* names
* nick
* notice
* oper
* part
* pass
* ping
* pong
* privmsg
* quit
* server
* squit
* stats
* time
* topic
* trace
* user
* version
* wallops
* who
* whois
<img src="https://github.com/markveligod/ft_irc/blob/main/img/server.gif">  
  
### 📄 Run bot
A bot named POGODA shows the current weather in the city. To run it, you need to run the `make bot` command. By default it will compile and join the local server with port 1080. For an example of how to access the bot via our client, see the Run client section.  
❗Attention: to run, you may need to install and connect third-party libraries. Be careful ☢️

<img src="https://github.com/markveligod/ft_irc/blob/main/img/bot.gif">  
  
### 📄 Run client
Also as a bonus we implemented the client part written in the qt framework.  
To interact with the bot, use a command like `PRIVMSG pogoda <City>`  
If you use a third-party chat, such as xchat, then use a command like `/pogoda <City>`  
<img src="https://github.com/markveligod/ft_irc/blob/main/img/client.gif">  
  
## 🏷️ RFC documentation
[RFC 1459](https://tools.ietf.org/html/rfc1459)  
[RFC 2810](https://tools.ietf.org/html/rfc2810)  
[RFC 2811](https://tools.ietf.org/html/rfc2811)  
[RFC 2812](https://tools.ietf.org/html/rfc2812)  
[RFC 2813](https://tools.ietf.org/html/rfc2813)  
[RFC 7194](https://tools.ietf.org/html/rfc7194)  
  
## 📫 Other
**:copyright:Authors:**  
  
*[Mark Veligod](https://github.com/markveligod)*  
*[polinariabar](https://github.com/polinariabar)*  
*[poringdol](https://github.com/poringdol)*  

