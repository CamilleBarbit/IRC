#include <sys/socket.h>
#include <stdio.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include "./utils.hpp"


// void forwardMessage(fd_set* clientSockets, int senderSocket, const char* message, int messageSize) {
//     for (int i = 0; i < FD_SETSIZE; i++) {
//         if (i != senderSocket && FD_ISSET(i, clientSockets)) {
//             if (send(i, message, messageSize, 0) == -1) {
//                 std::cerr << "Error sending message to client " << i << std::endl;
//             }
//         }
//     }
// }

int	main(int argc, char **argv) {

    /*  Step 1 : Verify the arguments */

    if (argc != 3)
    {
        std::cerr << "Error : Wrong number of arguments." << std::endl;
        return (1);
    }

    int sockport = 8080; //to test
    // sockport = myAtoi(argv[1]); //corresponds to the first argument after the executable (the port number)
    
    // if (sockport < 1024 || sockport > 65536)
    // {
    //     std::cerr << "Error : Wrong port number." << std::endl;
    //     return (1);
    // }

    /* Step 2 : Create master socket - the always listening socket */

    /* Socket creation on the server side - returns a file descriptor */
    int listening_sockfd; //listening variable

    if ((listening_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    //AF_INET corresponds to IPv4 and SOCK_STREAM corresponds to TCP

        std::cerr << "Error : Server socket could not be created." << std::endl;
        return (1);
    }
    else
    {
        std::cout << "TCP server socket was successfully created !" << std::endl;
        // std::cout << "My fd is : " << listening_sockfd << std::endl;
    }

    int opt = 1;
    //set listening socket to allow multiple connections
    // if (setsockopt(listening_sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt) == -1))
    // {
    //     std::cerr << "Error : setsockopt() did not worl." << std::endl;
    //     return (1);
    // }
    
    /* Step 3 : Bind the ip address and the port to the socket */
    
    struct sockaddr_in  server_hint;
    int                 server_len = sizeof(server_hint);
    
    server_hint.sin_family = AF_INET; //corresponds to the address family (refers to either TCP or UDP type)
    server_hint.sin_port = htons(sockport); //host to network short
    server_hint.sin_addr.s_addr = inet_addr("127.0.0.1"); //corresponds to the IP address of my machine sincer both server and client are on my machine

    if (bind(listening_sockfd, (struct sockaddr*)&server_hint, (socklen_t)server_len) == -1)
    {
        std::cerr << "Error : Server socket could not be bound to port." << std::endl;
        return (1);
    }
    else
        std::cout << "Server socket was successfully bound to port number : " << sockport << " !" << std::endl;
    
    // sockport = ntohs(server_hint.sin_port);
    // std::cout << "My port number is: " << sockport << std::endl;
    
    /* Step 4 : Listen to client request */

    if (listen(listening_sockfd, SOMAXCONN) == -1) //SOMAXCONN corresponds to the maximum number of connections we allow to the socket 
    {
        std::cerr << "Error : Server socket could not listen." << std::endl;
        close(listening_sockfd);
        return (1);
    }
    else
        std::cout << "Server socket is now listening and waiting for connections..." << std::endl;
    
    /* Step 5 : Wait and accept client request */


    int             new_socket, socket_desc;
    int             data_exchange;
    char            buffer[1024];
    
    fd_set current_sockets, readyToRead_sockets; //set of socket descriptors

    
    //clear the socket set
    FD_ZERO(&current_sockets); //clears all files from set readfds
    
    //add master socket to set
    FD_SET(listening_sockfd, &current_sockets);
    // max_fd = listening_sockfd;

    while (true) {

        readyToRead_sockets = current_sockets; //current_sockets corresponds to the set of file descriptors I want to keep an eye on

        if (select(FD_SETSIZE, &readyToRead_sockets, NULL, NULL, NULL) < 0)
        {
            std::cerr << "Error : Problem with file descriptor set." << std::endl;
            return (1);
        }
        //at that point, readyToRead_sockets contains all the file descriptors that are ready for reading
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            //we start from 0 and check which one is ready
            if(FD_ISSET(i, &readyToRead_sockets))
            {
                if (i == listening_sockfd) {
                //if the file descriptor i is the server socket, it means there is a new connection incoming
                    if ((new_socket = accept(listening_sockfd, (struct sockaddr*)&server_hint,
                        (socklen_t*)&server_len)) == -1)
                    {
                        std::cerr << "Error : Server socket could not accept connection." << std::endl;
                        return (1);        
                    }
                    printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(server_hint.sin_addr) , ntohs(server_hint.sin_port));
                    //the new socket is added to current_sockets (the set of sockets we want to keen an eye on)
                    FD_SET(new_socket, &current_sockets);
                }
                else 
                {
                    //the socket is an already existing client socket ready to be read
                    memset(buffer, 0, 1024);
                    int bytes_received = recv(i, buffer, sizeof(buffer), 0);
                    if (bytes_received == -1) {
                        std::cerr << "Error: Connection issue while exchanging messages." << std::endl;
                        close(i);
                        FD_CLR(i, &current_sockets);
                    }
                    else if (bytes_received == 0) {
                    // Client disconnected
                        printf("Client disconnected, socket fd is %d\n", i);
                        close(i);
                        FD_CLR(i, &current_sockets);
                    }
                    else {
                        std::cout << "Message received from socket " << i << ": " << buffer << std::endl;
                        //sending back the message to other clients
                        send(i, buffer, sizeof(buffer), MSG_CONFIRM);
                        // FD_CLR(i, &current_sockets);
                        // forwardMessage(&current_sockets, i, buffer, bytes_received);
                    // Optionally, you can send a response back to the client
                    // const char* response = "Message received";
                    // send(i, response, strlen(response), 0);
                    }                    
                }
            }
        }
    }
    close(listening_sockfd);
    return (0);
}
