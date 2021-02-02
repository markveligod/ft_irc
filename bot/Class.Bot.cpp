#include <sys/wait.h>
#include "Class.Bot.hpp"

void Bot::
push_cmd_queue(int fd, const string& str)
{
	_command_queue.push(std::make_pair(fd, str));
}

void Bot::
init_fd_select()
{
	FD_ZERO(&_fd_set_read);
	FD_ZERO(&_fd_set_write);
	for (map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end(); it++)
		FD_SET(it->first, &_fd_set_read);
	while (!_command_queue.empty())
	{
		if (_array_fd_select.count(_command_queue.front().first))
		{
			FD_SET(_command_queue.front().first, &_fd_set_write);
			break;
		}
		else
			_command_queue.pop();
	}
}

void Bot::
do_select()
{
	if ((_select_res = select(FD_SETSIZE, &_fd_set_read, &_fd_set_write, NULL, NULL)) < 0)
	{
		utils::print_error(ERR_SELECT, "SELECT");
	}
}

void Bot::
check_fd_select()
{
	for (map<int, int>::iterator it = _array_fd_select.begin(); it != _array_fd_select.end() && _select_res > 0; it++)
	{
		if (FD_ISSET(it->first, &_fd_set_write))
		{
			send(it->first, _command_queue.front().second.c_str(), strlen(_command_queue.front().second.c_str()), 0);
			_command_queue.pop();
		}

		if (FD_ISSET(it->first, &_fd_set_read))
		{
			if (it->second == FD_CLIENT)
			{
				char buffer[512 + 1];
				int n = recv(it->first, buffer, 512, 0);

				if (n < 1)
				{
					it = _array_fd_select.begin();
					continue;
				}
				buffer[n] = '\0';

				string buffer_cmd(buffer);
				if (buffer_cmd.size())
					utils::print_line("Recieved message " + buffer_cmd);

				Command mess(buffer_cmd);
				if (mess.getCommand() == "PRIVMSG")
				{
					string prefix = ":pogoda PRIVMSG " + mess.getPrefix() + " :";

					if (mess.getArgs().size() < 2 || mess.getArgs()[1] == "\r\n")
						return this->push_cmd_queue(it->first, prefix + "Введите город\r\n");

					pid_t pid = fork();

					if (pid < 0)
						utils::exit_error(1, "fork error");
					else if (pid == 0)
						get_response(mess.getArgs()[1]);
					else
					{
						wait(0);
						std::ifstream file("weather.json");
						if (!file.is_open())
							utils::exit_error(1, "file open error");
						json js;
						file >> js;
						file.close();

						if (js["cod"] == "404")
						{
							this->push_cmd_queue(it->first, prefix + "Error 404\r\n");
							this->push_cmd_queue(it->first, prefix + "Попробуй еще раз (^_^)\r\n");
						}
						else if (js["cod"] == 200)
						{
							std::vector<std::string> weather = get_picture(js["weather"][0]["main"]);

							this->push_cmd_queue(it->first, prefix + "******************************\r\n");

							std::string country = js["sys"]["country"];
							this->push_cmd_queue(it->first, prefix + weather[0] + "Country: " + country + "\r\n");

							std::string city = js["name"];
							this->push_cmd_queue(it->first, prefix + weather[1] + "City:    " + city + "\r\n");

							std::string weat = js["weather"][0]["main"];
							this->push_cmd_queue(it->first, prefix + weather[2] + "Weather: " + weat + "\r\n");

							std::string temp = utils::int_to_str((float)js["main"]["temp"] + KELVIN);
							this->push_cmd_queue(it->first, prefix + weather[3] + "Temp:    " + temp + "°C\r\n");

							std::string wind = utils::int_to_str((float)js["wind"]["speed"]);
							this->push_cmd_queue(it->first, prefix + weather[4] + "Wind:    " + wind + " m/s\r\n");

							this->push_cmd_queue(it->first, prefix + "******************************\r\n");
						}
					}
				}
				bzero(buffer, 512);
				break;
			}
			_select_res--;
		}
	}
}

int Bot::
get_response(string city)
{
	char* args[2];
	args[0] = strdup(WEATHER_SCRIPT);
	args[1] = NULL;

	char* env[2];
	city[city.size() - 2] = '\0';
	env[0] = strdup(("CITY=" + city).c_str());
	env[1] = NULL;
	
	if (execve(args[0], args, env) < 0)
		utils::exit_error(1, "execve error");
	return 0;
}

std::vector<std::string> Bot::
get_picture(std::string weather)
{
	std::vector<string> res(5, string());

	if (weather == "Clear")
	{
		res[0] = "     \\   /     ";
		res[1] = "      .-.      ";
		res[2] = "   ― (   ) ―   ";
		res[3] = "      `-’      ";
		res[4] = "     /   \\     ";
	}

	else if (weather == "Clouds")
	{
		res[0] = "               ";
		res[1] = "      .--.     ";
		res[2] = "   .-(    ).   ";
		res[3] = "  (  .  )  )   ";
		res[4] = "   ‾‾ ‾‾ ‾‾    ";
	}

	else if (weather == "Mist" || weather == "Fog")
	{
		res[0] = "               ";
		res[1] = "    -   -   -  ";
		res[2] = "  ‾  -‾  -‾    ";
		res[3] = "   ‾-  ‾-  ‾-  ";
		res[4] = "  ‾   ‾   ‾    ";
	}

	else if (weather == "Snow")
	{
		res[0] = "      .-.      ";
		res[1] = "     (   ).    ";
		res[2] = "    (__(__)    ";
		res[3] = "    * * * *    ";
		res[4] = "   * * * *     ";
	}

	else if (weather == "Rain")
	{
		res[0] = "      .-.      ";
		res[1] = "     (   ).    ";
		res[2] = "    (___(__)   ";
		res[3] = "   ‚‘‚‘‚‘‚‘    ";
		res[4] = "   ‚’‚’‚’‚’    ";
	}
	else
	{
		res[0] = "               ";
		res[1] = "               ";
		res[2] = "               ";
		res[3] = "               ";
		res[4] = "               ";
	}
	return res;
}