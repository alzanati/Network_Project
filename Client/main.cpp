#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

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
void TrimEnd (char* buffer) {
    int len = (int) strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = 0;
}

/**
 * Open a client on this port
 * @param port
 */
void Client(const char* port) {
    /* Attributes */
    int sockFd, portNo, n;
    struct in_addr ipv4addr;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char buffer[256];

    /* Create file descriptor */
    portNo = atoi(port);
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0)
        error("ERROR opening socket");

    /* Get host address */
    inet_pton(AF_INET, "127.0.0.1", &ipv4addr);
    server = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    printf("%s\n", server->h_name);

    /* Copy host data to server_addr object */
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy(server->h_addr, (char *)&serverAddress.sin_addr.s_addr, (size_t) server->h_length);
    serverAddress.sin_port = htons((uint16_t) portNo);

    /* Connect socket to a name */
    if (connect(sockFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        error("ERROR connecting");

    /* Get client name */
    printf("Your Name: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    /* Send message */
    n = (int) write(sockFd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    /* Read clear buffer and read message */
    while (true) {

        /* Close connection */
        if (IsExit(buffer)) {
            close(sockFd);
            break;
        }

        /* Get response from server */
        bzero(buffer, 256);
        n = (int) read(sockFd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        std::cout << "Server: " << buffer << "\n";

        /* Send message */
        std::cout << "You : ";
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = (int) write(sockFd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
    }
}

int main() {
    Client("51718");
    return EXIT_SUCCESS;
}