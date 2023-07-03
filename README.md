# My Own Internet Relay Chat (IRC)

## Important variables

1) Define the server's port,
2) Create the master socket (aka the always listening socket) -> This variable is nothing else thant the file decriptor returned by the function socket().


## When does it all start ?

After creating the always listening socket, it is fundamental to **bind** it. Indeed,when a socket is created, it exists in a name space (address family) but has so address assigned to it. bind() assigns the address specified by the argument addr to the socket referred to by the file descriptor sockfd.

**prototype** : _int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)_;


When the socket is properly bound, it is ready to **listen** to any client request coming from the other end of the socket.

**prototype** : _int listen(int sockfd, int backlog)_;

_The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow (aka the total number of clients that the server is able to deal with at the same time)_.


## Ready To Get Tricky !

That is now the time to wait and accept client requests...In this context the select() function is of particular interest.

**prototype** : _int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)_;