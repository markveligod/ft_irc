
/*
**==========================
** Class Command - Класс реализует логику распарсинга данных по командам:
** PASS - используется для установки 'парольного соединения' 
** NICK - используется для установки пользовательского никнейма или изменения предыдущего
**==========================
*/

#pragma once
#include "main.hpp"

using std::string;
using std::vector;
using std::map;

class IRC;
class Server;
class Channel;

class Command
{
private:
	string			_prefix;
	string			_command;
	vector<string>	_arguments;
	string			_message;

public:
	Command(const string& str);
	~Command();

	const string&			getCommand() const;
	const string&			getMessage() const;
	const string&			getPrefix() const;
	const vector<string>&	getArgs() const;
};