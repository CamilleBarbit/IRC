#ifndef UTILS_HPP
# define UTILS_HPP


#include <sys/socket.h>
#include <stdio.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>

/* Structure */

typedef struct  s_infos {

    int                 server_sockfd;
    int                 server_sockport;
    std::vector<int>    all_sockfds;
    fd_set              original_fd, ready_fd; //set of socket descriptors

} t_infos;


/* Functions */

int     myAtoi(const char *str);
int	    mylen(char *buff);
int     run_server(t_infos *key_infos);
int     handle_request(t_infos *key_infos, int client_fd);
int     new_client(t_infos *key_infos);
int     start_server(t_infos *key_infos);
void    erase_vector_value(t_infos *key_infos, int client_fd);
void    send_message(t_infos *key_infos, int sender_fd, char *msg);
void    decorticate_message(int targeted_client, char *msg);


#endif