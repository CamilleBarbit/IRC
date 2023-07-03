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

To understand select(), it basically allows a program (our server) to monitor multiple file descriptors, waiting until one or more of the file descriptors is considered "ready" for some class of I/O operation (e.g. read(), write() operations).

The fd_set type of variables requires some deeper explanations...The select() function takes three "sets" of file descriptors (declared as fd_set) which allow the caller to wait for three classes of events in the specified set of file descriptors.

Here are some macros to work with fd_sets...

**prototype** : _FD_ZERO()_ -> This macro clears (removes all file descriptors from) set. It should be employed as the first step in initializing a file descriptor set.

**prototype** : _FD_SET()_ -> This macro adds the file descriptor fd to set. Adding a file descriptor that is already present in the set is a no-op, and does not produce an error.

Note that select() is DESTRUCTIVE, meaning that it changes the fd_set (passed as parameter).