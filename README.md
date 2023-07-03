# IRC

# Important variables

    1) Define the server's port,
    2) Create the master socket (aka the always listening socket) -> This variable is nothing else thant the file decriptor returned by the function socket().


## When does it all start ?

    After creating the always listening socket, it is fundamental to bind it. Indeed,when a socket is created, it exists in a name space (address family) but has so address assigned to it. bin() assigns the address specified by the argument addr to the socket referred to by the file descriptor sockfd.

    prototype : *int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)*;



