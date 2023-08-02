#include "./server.hpp"


#define BUFF_SIZE 1024
#define ADDRESS "127.0.0.1"

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
        std::cout << "Server socket was successfully bound to port number : " << key_infos->server_sockport << " !" << std::endl;
    
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

void    decorticate_message(int targeted_client, char *msg)
{
    int sent;
	int already_sent = 0;
	int msg_len = strlen(msg); //length of the message to be sent

	while (already_sent < msg_len)
    {
		if ((sent = send(targeted_client, msg + already_sent, msg_len - already_sent, MSG_DONTWAIT)) <= 0)
			return ;
		already_sent += sent;
	}  
}

//sending the message to every client that is not the sender !
void    send_message(t_infos *key_infos, int sender_fd, char *msg)
{
    for (unsigned int i = 0; i < key_infos->all_sockfds.size(); i++)
    {
        if (key_infos->all_sockfds[i] != sender_fd)  
            decorticate_message(key_infos->all_sockfds[i], msg);
    }

}

int new_client(t_infos *key_infos)
{
    struct sockaddr_in  client_hint;
    socklen_t           client_len = sizeof(client_hint);
    int                 new_sockfd;
    char                welcome_msg[100];

    if ((new_sockfd = accept(key_infos->server_sockfd, (struct sockaddr*)&client_hint, (socklen_t*)&client_len)) == -1)
    {
        std::cerr << "Error : Server socket could not accept new connection." << std::endl;
        return (-1);        
    }
    printf("New connection, client socket fd is %d , ip is : %s , port : %d \n", new_sockfd, inet_ntoa(client_hint.sin_addr), ntohs(client_hint.sin_port));
    
    //the new socket is added to original_fd (the set of sockets we want to keen an eye on)
    FD_SET(new_sockfd, &key_infos->original_fd);
    
    key_infos->all_sockfds.push_back(new_sockfd);
    
    //writing a message to let all clients know a new client successfully connected
    sprintf(welcome_msg, "From server: Client %d successfully connected to the server !\n", new_sockfd);
    send_message(key_infos, new_sockfd, welcome_msg);
    
    return (0);
}

void    erase_fd(t_infos *key_infos, int client_fd)
{
    int j = 0;
    for (unsigned int i = 0; i < key_infos->all_sockfds.size(); i++)
    {
        if (client_fd == key_infos->all_sockfds[i])
            key_infos->all_sockfds.erase(key_infos->all_sockfds.begin()+j);
        j++;
    }
}

int handle_request(t_infos *key_infos, int client_fd)
{
    char    buffer[BUFF_SIZE];
    int     bytes_received = recv(client_fd, buffer, BUFF_SIZE - 1, 0);

    if (bytes_received == -1) {
        std::cerr << "Error: Connection issue while exchanging messages." << std::endl;
        erase_fd(key_infos, client_fd);
        FD_CLR(client_fd, &key_infos->original_fd); //clearing that socket from the list of sockets I am monitoring
        close(client_fd);
        return (-1);
    }

    if (bytes_received == 0)
    {
        char    goodbye_msg[100];
        printf("Client disconnected, its socket fd is %d\n", client_fd);
        //writing a message to let all clients know a new client successfully disconnected
        sprintf(goodbye_msg, "From server: Client %d successfully disconnected from the server !\n", client_fd);
        send_message(key_infos, client_fd, goodbye_msg);
        erase_fd(key_infos, client_fd);
        FD_CLR(client_fd, &key_infos->original_fd); //clearing that socket from the list of sockets I am monitoring
        close(client_fd);
        return (0);
    }
    
    //getting ready to read data
    buffer[bytes_received] = '\0';
    std::cout << "From client " << client_fd << " : " << buffer << std::endl;
    send_message(key_infos, client_fd, buffer);

    return(0);

}

int run_server(t_infos *key_infos)
{

    FD_ZERO(&key_infos->original_fd); //to clear all file descriptors from the set
    FD_SET(key_infos->server_sockfd, &key_infos->original_fd); //add the listening socket to the original set of file descriptors

    while (true)
    {
        key_infos->ready_fd = key_infos->original_fd;

        if (select(FD_SETSIZE, &key_infos->ready_fd, NULL, NULL, NULL) == -1)
        {
            std::cerr << "Error : Problem with file descriptor set." << std::endl;
            return (-1);
        }

        if (FD_ISSET(key_infos->server_sockfd, &key_infos->ready_fd))
        {    //new client
            if (new_client(key_infos) == -1)
                return (-1);
        
        }

        else
        {
            for (unsigned int i = 0; i < key_infos->all_sockfds.size(); i++)
            {
                if (FD_ISSET(key_infos->all_sockfds[i], &key_infos->ready_fd))
                {    
                    if (handle_request(key_infos, key_infos->all_sockfds[i]) == -1)
                        return (-1);
                }
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
    
    t_infos  *key_infos = new t_infos;

    key_infos->server_sockport = sock_port;

    if (start_server(key_infos) == -1 || run_server(key_infos) == -1)
    {
        delete key_infos;
        return (1);
    }

    delete key_infos;
    return (0);
}