#include <stdio.h>              // For standard I/O functions
#include <sys/types.h>          // For system types
#include <sys/socket.h>         // For socket functions
#include <netinet/in.h>         // For sockaddr_in structure (Internet addresses)
#include <arpa/inet.h>          // For functions related to Internet operations
#include <signal.h>             // For signal handling
#include "tcp.h"                // Custom header file for constants and message structure

int	Sockfd;                     // Global socket file descriptor for the server
int on;                         // For setsockopt

// Function to gracefully close the server upon receiving a termination signal (e.g., Ctrl+C)
void CloseServer() {
    close(Sockfd);              // Close the socket
    printf("\nTCP Server exit.....\n");
    exit(0);                    // Exit the program
}

int main(int argc, char *argv[]) {
    int newSockfd;              // Socket descriptor for accepted connection
    int cliAddrLen, n;          // Client address length and bytes read
    struct sockaddr_in cliAddr, servAddr; // Structures to hold client and server addresses
    MsgType msg;                // Message structure defined in tcp.h

    signal(SIGINT, CloseServer); // Register signal handler for SIGINT (Ctrl+C)

    // Step 1: Create a TCP socket
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");       // Print error if socket creation fails
        exit(1);
    }

    // Step 2: Initialize server address structure
    on = 1;                     // Allow the socket to be reused immediately after the server terminates
    if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    bzero((char *)&servAddr, sizeof(servAddr)); // Clear the structure
    servAddr.sin_family = PF_INET;             // Set family to Internet
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to any IP address
    servAddr.sin_port = htons(SERV_TCP_PORT);  // Set port number from tcp.h

    // Step 3: Bind the socket to the server address
    if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("bind");         // Print error if binding fails
        exit(1);
    }

    // Step 4: Set the socket to listen for incoming connections
    listen(Sockfd, 5);          // Allow a backlog of 5 connections

    printf("TCP Server started.....\n");

    // Step 5: Server main loop to accept and handle client connections
    cliAddrLen = sizeof(cliAddr); // Initialize client address length
    while (1) {
        // Accept a new client connection
        newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
        if (newSockfd < 0) {
            perror("accept");   // Print error if accept fails
            exit(1);
        }
        
        // Step 6: Read the message sent by the client
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0) {
            perror("read");     // Print error if read fails
            exit(1);
        }
        printf("Received request: %s.....", msg.data); // Print received message

        // Step 7: Prepare and send the reply
        msg.type = MSG_REPLY;   // Set message type to reply
        sprintf(msg.data, "This is a reply from %d.", getpid()); // Create reply message
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
            perror("write");    // Print error if write fails
            exit(1);
        }
        printf("Replied.\n");

        usleep(10000);          // Brief pause before closing connection
        close(newSockfd);       // Close the client connection
    }
}
