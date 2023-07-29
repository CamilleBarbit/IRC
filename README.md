# My Own Internet Relay Chat (IRC)

## Important Variables

1) The server's port (type : _int_).
2) The master socket (aka the always listening socket) -> This variable is nothing else thant the file decriptor returned by the function socket() (type : _int_).
3) The sockaddr_in structure -> This variable enables to bind the IP address and the port to the socket.

## Other Variables

1) SOMAXCONN is a global variable that corresponds to the maximum number of pending connection requests queued for any listening socket. The minimum value is 1, the maximum value is 2147483647, and the default value is 1024


## When Does It All Start ?

After creating the always listening socket, it is fundamental to **bind** it. Indeed,when a socket is created, it exists in a name space (address family) but has so address assigned to it. bind() assigns the address specified by the argument addr to the socket referred to by the file descriptor sockfd.

**prototype** : _int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)_;


When the socket is properly bound, it is ready to **listen** to any client request coming from the other end of the socket.

**prototype** : _int listen(int sockfd, int backlog)_;

_The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow (aka the total number of clients that the server is able to deal with at the same time). In this case, we use the global variable SOMAXCONN as backlog_.


## Ready To Get Tricky !

That is now the time to **wait and accept** client requests...In this context the select() function is of particular interest.

**prototype** : _int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)_;

To understand select(), it basically allows a program (our server) to monitor multiple file descriptors, waiting until one or more of the file descriptors is considered "ready" for some class of I/O operation (i.e. read(), write() operations). On success, the function returnss the number of file descriptors contained in the threz returned descriptor sets (readfs, writedfs, exceptfds). In our case, the third and fourth parameters are set to NULL.

The fd_set type of variables requires some deeper explanations...The select() function takes three "sets" of file descriptors (declared as _fd_set_) which allow the caller to wait for three classes of events in the specified set of file descriptors.

Here are some macros to work with fd_sets...

**prototype** : _FD_ZERO()_ -> This macro clears (removes all file descriptors from) set. It should be employed as the first step in initializing a file descriptor set.

**prototype** : _FD_SET()_ -> This macro adds the file descriptor fd to set. Adding a file descriptor that is already present in the set is a no-op, and does not produce an error.

Note that select() is DESTRUCTIVE, meaning that it changes the fd_set (passed as parameter).


### Deal With New Clients

FD_ISSET(**server_sockfd**, &ready_fd) is used to test if the **server_sockfd** (i.e. the server's file descriptor) is still in the readfds set once the select() function has been called.

**if (FD_ISSET(server_sockfd, ready_fd))**
    It means that a **new client** is trying to connect to the server !

>       int new_client(t_infos *key_infos)
>       {
>>       struct sockaddr_in  client_hint;
>>       socklen_t           client_len = sizeof(client_hint);
>>       int                 new_sockfd;
>>       char                new_msg[100];
>>
>>       if ((new_sockfd = accept(key_infos->server_sockfd, (struct sockaddr*)&client_hint, (socklen_t*)&client_len)) == -1)
>>       {
>>           std::cerr << "Error : Server socket could not accept new connection." << std::endl;
>>           return (-1);        
>>       }
>>       printf("New connection, client socket fd is %d , ip is : %s , port : %d \n", new_sockfd, inet_ntoa(client_hint.sin_addr), ntohs(client_hint.sin_port));
>>    
>>       //the new socket is added to original_fd (the set of sockets we want to keen an eye on)
>>       FD_SET(new_sockfd, &key_infos->original_fd);
>>       
>>       key_infos->all_sockfds.push_back(new_sockfd);
>>       for (int i = 0; i < key_infos->all_sockfds.size(); i++)
>>       {
>>           std::cout << "New in : " << key_infos->all_sockfds[i] << std::endl;
>>       }
>>       
>>       //writing a message to let all clients know a new client successfully connected
>>       sprintf(new_msg, "From server: Client %d successfully connected to the server !\n", new_sockfd);
>>       send_message(key_infos, new_sockfd, new_msg);
>>    
>>       return (0);
>       }

