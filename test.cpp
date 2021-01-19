#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdio.h>

int main()
{
	time_t t = time(0);
	std::cout << ctime(&t) << std::endl;  
}