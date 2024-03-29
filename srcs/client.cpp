#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include "./server.hpp"

int	main(int argc, char **argv) {

    int sockfd;

    int server_fd = myAtoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    //AF_INET corresponds to IPv4 and SOCK_STREAM corresponds to TCP
    {
        std::cerr << "Error : Client socket could not be created." << std::endl;
        return (1);
    }
    else
    {
        std::cout << "Client socket was successfully created !" << std::endl;
    }

    struct sockaddr_in  hint;
    int server_sockport = 8080; //to test, this must be the server's port

    hint.sin_family = AF_INET;
    hint.sin_port = htons(server_sockport);
    hint.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Error : Client socket could not be connected." << std::endl;
        return (1);
    }
    else
    {
        std::cout << "Client socket was successfully connected !" << std::endl;
        std::cout << "Client can start sending and receiving data..." << std::endl;
    }


    std::string message;
    char        buffer[1024];
    while (true) {

        //memset(buffer, 0, 1024); //clear buffer
        //memset(received, 0, 1024); //clear buffer
        bzero(buffer, 1024);

        std::getline(std::cin, message);

        strcpy(buffer, message.c_str());
        std::cout << "The buffer sent to server is: " << buffer << std::endl;
        send(server_fd, buffer, strlen(buffer), 0);
        
        message.clear();
        bzero(buffer, 1024);
        
        int bytes_received = recv(server_fd, buffer, sizeof(buffer), 0);
        buffer[bytes_received] = '\0';
    }

    return (0);
}
