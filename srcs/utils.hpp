#ifndef UTILS_HPP
# define UTILS_HPP


/* Structure */

typedef struct  s_infos {

    int                 server_sockfd;
    int                 server_sockport;
    std::vector<int>    all_sockfds;

} t_infos;


/* Functions */

int myAtoi(const char *str);
int	mylen(char *buff) ;

#endif