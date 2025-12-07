#include <stdio.h>              // For standard I/O functions
#include <sys/types.h>          // For system types (e.g., pid_t)
#include <sys/socket.h>         // For socket system calls
#include <netinet/in.h>         // For sockaddr_in structure (Internet addresses)
#include <arpa/inet.h>          // For inet functions (e.g., inet_addr)
#include "tcp.h"                // Custom header for port, address, and message structure definitions

int main(int argc, char *argv[]) {
    int sockfd, n;                       // Socket file descriptor and variable to store the number of bytes read
    struct sockaddr_in servAddr;         // Structure to hold server address information
    MsgType msg;                         // Message structure defined in tcp.h to store data to send/receive

    // Step 1: Create a TCP socket
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");                // Print error message if socket creation fails
        exit(1);                         // Exit the program with an error
    }

    // Step 2: Initialize server address structure
    bzero((char *)&servAddr, sizeof(servAddr)); // Clear the server address structure
    servAddr.sin_family = PF_INET;             // Set address family to Internet
    servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // Set server IP address (from tcp.h)
    servAddr.sin_port = htons(SERV_TCP_PORT);  // Set server port number (from tcp.h, in network byte order)

    // Step 3: Connect to the server
    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect");               // Print error if connection to the server fails
        exit(1);                         // Exit with an error
    }

    // Step 4: Prepare a request message to send to the server
    msg.type = MSG_REQUEST;             // Set message type to "request" (from tcp.h)
    sprintf(msg.data, "This is a request from %d.", getpid()); // Populate the message data with the process ID

    // Step 5: Send the request message to the server
    if (write(sockfd, (char *)&msg, sizeof(msg)) < 0) {
        perror("write");                // Print error if the write operation fails
        exit(1);                        // Exit with an error
    }
    printf("Sent a request.....\n");

    // Step 6: Read the reply from the server
    if ((n = read(sockfd, (char *)&msg, sizeof(msg))) < 0) {
        perror("read");                 // Print error if the read operation fails
        exit(1);                        // Exit with an error
    }
    printf("Received reply: %s\n", msg.data); // Print the received reply message

    // Step 7: Close the socket and terminate
    close(sockfd);                     // Close the connection socket
    return 0;                          // Exit successfully
}
