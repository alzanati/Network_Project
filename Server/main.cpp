#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/**
 *
 * @param msg
 */
void error(const char *msg) {
    perror(msg);
    exit(1);
}

/**
 *
 * @param msg
 * @return
 */
bool IsExit(const char* msg) {
    return msg[0] == 'e' && msg[1] == 'x' &&
    msg[2] == 'i' && msg[3] == 't';
}

/**
 *
 * @param buffer
 */
void TrimEnd(char* buffer) {
    int len = (int) strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = 0;
}

/**
 * Open a server on this port
 * @param port
 */
void Server(const char* port) {
    /* Attributes */
    int sockFd, portNo, newSockFd;
    socklen_t clientLength;
    char buffer[256];
    char clientName[256];
    struct sockaddr_in serverAdderss, clientAddress;
    int n;

    /* Open socket */
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0)
        error("ERROR opening socket");

    /* Initialize server address to zero */
    bzero((char *) &serverAdderss, sizeof(serverAdderss));

    /* Create name */
    portNo = atoi(port);
    serverAdderss.sin_family = AF_INET;
    serverAdderss.sin_port = htons((uint16_t) portNo);
    serverAdderss.sin_addr.s_addr = INADDR_ANY;
    //inet_aton("127.0.0.1", (in_addr *) &serverAdderss.sin_addr.s_addr); // set IpAddress for machine

    /* Assigning a name to a socket and listen to it */
    bind(sockFd, (struct sockaddr *) &serverAdderss, sizeof(serverAdderss));
    listen(sockFd, 5);

    while (true) {

        /* Close Server */
        std::cout << "write exit to close server or press continue...\n";
        char closeServer[4];
        bzero(closeServer, 4);
        fgets(closeServer, 5, stdin);
        if (IsExit(closeServer)) {
            close(sockFd);
            break;
        }

        /* Accept client connection */
        clientLength = sizeof(clientAddress);
        newSockFd = accept(sockFd, (struct sockaddr *) &clientAddress, &clientLength);
        if (newSockFd < 0)
            error("ERROR on accept");

        /* Get client name */
        bzero(clientName, 256);
        n = (int) read(newSockFd, clientName, 255);
        if (n < 0)
            error("ERROR reading from socket");

        /* Show client readable name */
        TrimEnd(clientName);
        std::cout << "Chating with " << clientName << "\n\n";

        /* Confirm acceptance of connection */
        n = (int) write(newSockFd, "Connection accepted ", strlen("Connection accepted "));
        if (n < 0)
            error("ERROR writing to socket");

        /* Conversion */
        while (true) {

            /* Read messages from client */
            bzero(buffer, 256);
            n = (int) read(newSockFd, buffer, 255);
            if (n < 0)
                error("ERROR reading from socket");

            /* Close connection for client */
            if (IsExit(buffer) || (n == 0)) {
                close(newSockFd);
                std::cout << "Client " << clientName << " close the connection!!! \n";
                break;
            }

            /* Show client message */
            if (!(n == 1 && buffer[0] == '\n'))
                std::cout << clientName << " : " << buffer << "\n";

            /* Send to client */
            std::cout << "Server: ";
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            n = (int) write(newSockFd, buffer, 255);
            if (n < 0)
                error("ERROR writing to socket");
        }
    }
}

int main() {
    Server("51718");
    return EXIT_SUCCESS;
}