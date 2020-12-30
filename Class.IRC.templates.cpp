/*
** ------------------------------------------------------------------
** find_fd			- находит в векторе итераторную позицию которая 
**           		  соответствует переданный fd
**					- вместо T можно подавать Client * или User *
** ------------------------------------------------------------------
** find_nickname	- находит в векторе итераторную позицию которая 
**           		  соответствует переданному никнейму
**					- вместо T можно подавать Client * или User *
** ------------------------------------------------------------------
*/

template <typename T>
int IRC::find_nickname(T& cont, std::string const &nickname)
{
	typename T::iterator v_begin = cont.begin();
	int i = 0;

	while (v_begin != cont.end())
	{
		if ((*v_begin)->getNickname() == nickname)
			return (i);
		v_begin++;
		i++;
	}
	return (-1);
}

template <typename T>
int IRC::find_fd(T& cont, int fd)
{
	typename T::iterator v_begin = cont.begin();
	int i = 0;

	while (v_begin != cont.end())
	{
		if ((*v_begin)->getSocketFd() == fd)
			return (i);
		v_begin++;
		i++;
	}
	return (-1);
}