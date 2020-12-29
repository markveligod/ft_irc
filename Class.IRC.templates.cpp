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
int IRC::find_nickname(std::vector<T> *vect, std::string const &nickname)
{
	typename std::vector<T>::iterator v_begin = (*vect).begin();
	typename std::vector<T>::iterator v_end = (*vect).end();
	int i = 0;

	while (v_begin != v_end)
	{
		if ((*v_begin)->getNickname() == nickname)
			return (i);
		v_begin++;
		i++;
	}
	return (-1);
}

template <typename T>
int IRC::find_fd(std::vector<T> *vect, int fd)
{
	typename std::vector<T>::iterator v_begin = (*vect).begin();
	typename std::vector<T>::iterator v_end = (*vect).end();
	int i = 0;

	while (v_begin != v_end)
	{
		if ((*v_begin)->getSocketFd() == fd)
			return (i);
		v_begin++;
		i++;
	}
	return (-1);
}