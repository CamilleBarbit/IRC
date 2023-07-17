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

#include "./utils.hpp"


#define BUFF_SIZE = 1024
#define ADDRESS = "127.0.0.1"

void    forwardMessage(char *msg, int receiver)
{
  	// int ret;
	// int already_sent = 0;
	// int msg_len = strlen( msg );

	// while ( already_sent < msg_len ) {
	// 	if ( !send(receiver, msg + already_sent, msg_len - already_sent, MSG_DONTWAIT) )
	// 		return ;
	// 	already_sent += ret;
	// }  
}

// int check_args(int argc, char **argv) {

//     if (argc != 3)
//     {
//         std::cerr << "Error : Wrong number of arguments." << std::endl;
//         return (-1);
//     }

//     int port = myAtoi(argv[1]); //server's port number

//     if ( port < 1024 ||  port > 65536)
//     {
//         std::cerr << "Error : Wrong port number." << std::endl;
//         return (-1);
//     }

//     std::cout << "Server can be initiated !" << std::endl;
//     return (port); //if everything is fine with the arguments, then I return the port number  

// }

int    start_server(t_infos *key_infos)
{
    /* Assign listening socket */
    
    if ((key_infos->server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {

        std::cerr << "Error : Server socket could not be created." << std::endl;
        return (-1);
    }
    else
    {
        std::cout << "TCP server socket was successfully created !" << std::endl;
        std::cout << "My fd is : " << key_infos->server_sockfd << std::endl;
    }

    /* Bind ip address and port to the socket */
    
    struct sockaddr_in  server_hint;
    int                 server_len = sizeof(server_hint);
    
    server_hint.sin_family = AF_INET;
    server_hint.sin_port = htons(key_infos->server_sockport);
    server_hint.sin_addr.s_addr = inet_addr(ADDRESS);
    
    if (bind(key_infos->server_sockfd, (struct sockaddr*)&server_hint, (socklen_t)server_len) == -1)
    {
        std::cerr << "Error : Server socket could not be bound to port." << std::endl;
        return (-1);
    }
    else
        std::cout << "Server socket was successfully bound to port number : " << port << " !" << std::endl;
    
    // port = ntohs(server_hint.sin_port);
    // std::cout << "My port number is: " << port << std::endl;
    
    /* Listen to client request */

    if (listen(key_infos->server_sockfd, SOMAXCONN) == -1)
    {
        std::cerr << "Error : Server socket could not listen." << std::endl;
        close(key_infos->server_sockfd);
        return (-1);
    }
    else
        std::cout << "Server socket is now listening and waiting for connections..." << std::endl;
     
    return (0); //if everything is fine with the arguments, then I return the fd of the listening socket
}

int new_client(t_infos *key_infos, fd_set *original_fd)
{
    struct sockaddr_in  client_hint;
    socklen_t           client_len = sizeof(client_hint);
    int                 new_sockfd;

    if ((new_sockfd = accept(key_infos->server_sockfd, (struct sockaddr*)&client_hint, (socklen_t*)&client_len)) == -1)
    {
        std::cerr << "Error : Server socket could not accept new connection." << std::endl;
        return (-1);        
    }
    printf("New connection, client socket fd is %d , ip is : %s , port : %d \n", new_sockfd, inet_ntoa(client_hint.sin_addr), ntohs(client_hint.sin_port));
    
    //the new socket is added to original_fd (the set of sockets we want to keen an eye on)
    FD_SET(new_sockfd, &original_fd);
    
    key_infos->all_sockfds.push_back(new_sockfd);
    // for (int i = 0; i < all_sockets.size(); i++)
    // {
    //     std::cout << "New in : " << all_sockets[i] << std::endl;
    // }
}

void    erase_vector_value(std::vector<int> all_sockets, int client_fd)
{
    for (std::vector<int>::iterator it = all_sockets.begin(); it != all_sockets.end(); it++)
    {
        if (*it == client_fd)
            all_sockets.erase(it);
    } 
}

void    decorticate_message(int targeted_client, char *msg)
{

}

void    send_message(std::vector<int> all_sockets, int sender_fd, char *msg)
{
    for (std::vector<int>::iterator it = all_sockets.begin(); it != all_sockets.end(); it++)
    {
        if (*it != sender_fd)
            decorticate_message(*it, msg);
    }

}

int handle_request(std::vector<int> all_sockets, int client_fd, fd_set *original_fd)
{
    char    buffer[BUFF_SIZE];
    int     bytes_received = recv(client_fd, buffer, 1023, MSG_DONTWAIT);

    if (bytes_received == -1) {
        std::cerr << "Error: Connection issue while exchanging messages." << std::endl;
        close(client_fd);
        FD_CLR(client_fd, &original_sockets); //clearing that socket from the list of sockets I am monitoring
        erase_vector_value(all_sockets, client_fd);
    }

    if (bytes_received == 0)
    {
        // Client disconnected
        printf("Client disconnected, its socket fd is %d\n", client_fd);
        close(client_fd);
        FD_CLR(client_fd, &original_sockets); //clearing that socket from the list of sockets I am monitoring 
        erase_vector_value(all_sockets, client_fd);
    }
    buffer[ret] = '\0';



}

int run_server(t_infos *key_infos)
{
    fd_set              original_fd, ready_fd; //set of socket descriptors

    FD_ZERO(&original_fd); //to clear all file descriptors from the set
    FD_SET(key_infos->server_sockfd, &original_fd); //add the listening socket to the original set of file descriptors

    while (true)
    {
        ready_fd = original_fd;

        if (select(FD_SETSIZE, &ready_fd, NULL, NULL, NULL) == -1)
        {
            std::cerr << "Error : Problem with file descriptor set." << std::endl;
            return (-1);
        }

        if (FD_ISSET(key_infos->server_sockfd, &ready_fd))
            //new client
            if (new_client(&key_infos, &original_fd) == -1)
                return (-1);

        else
        {
            for (std::vector<int>::iterator it = key_infos->all_sockfds.begin(); it != key_infos->all_sockfds.end(); it++)
            {
                if (FD_ISSET(*it, &ready_fd))
                    if (handle_request(key_infos->all_sockfds, i, &original_fd) == -1)
                        return (-1);
            }

        }
    }
    return (0);
}

int	main(int argc, char **argv) {

    /* Verify the arguments */
    int sock_port = 8080; //to test
    // int sock_port; //server's port
    
    // if((sock_port = check_args(argc, argv)) == -1)
    //     return (1);
    
    struct t_infos  key_infos;

    key_infos->server_sockport = sock_port;

    if (start_server(&key_infos) == -1)
        return (1);

    if (run_server(&key_infos) == -1)
        return (1);
    
    return (0);
}