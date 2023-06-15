#include "./utils.hpp"

int myAtoi(const char *str) {

    int i = 0;
    int nbr = 0;
    int sign = 1; //becomes -1 if it is a negative number

    //Ignore whitespaces
	while (str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\v' || str[i] == '\f' || str[i] == '\r'
		|| str[i] == ' ')
		i++;
    
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = sign * -1;
		i++;
	}

	while (str[i] >= '0' && str[i] <= '9')
	{
        nbr = nbr * 10 + str[i] - '0';
		i++;
	}

	return (nbr * sign);
}
