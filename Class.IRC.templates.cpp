/*
** ------------------------------------------------------------------
** find_fd			- находит в векторе итераторную позицию которая 
**           		  соответствует переданный fd
**					- вместо T можно подавать Client* или User*
** ------------------------------------------------------------------
** find_name	- находит в векторе итераторную позицию которая 
**           		  соответствует переданному никнейму
**					- вместо T можно подавать Client* или User*
** ------------------------------------------------------------------
*/

template <typename T>
int IRC::find_name(T& cont, const std::string& name)
{
	typename T::const_iterator v_begin = cont.begin();
	int i = 0;

	while (v_begin != cont.end())
	{
		if (is_equal((*v_begin)->getName(),name))
			return i;
		v_begin++;
		i++;
	}
	return -1;
}

template <typename T>
int IRC::find_fd(T& cont, int fd)
{
	typename T::const_iterator v_begin = cont.begin();
	int i = 0;

	while (v_begin != cont.end())
	{
		if ((*v_begin)->getSocketFd() == fd)
			return i;
		v_begin++;
		i++;
	}
	return -1;
}

template <typename T>
int IRC::find_fd_count(T &cont, int fd)
{
	typename T::const_iterator v_begin = cont.begin();
	int i = 0;

	while (v_begin != cont.end())
	{
		if ((*v_begin)->getSocketFd() == fd)
			i++;
		v_begin++;
	}
	return i;
}
