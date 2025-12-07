#include <stdio.h>              // Standard I/O functions
#include <stdlib.h>             // For exit()
#include <unistd.h>             // For fork(), close(), etc.
#include <sys/types.h>          // For system types
#include <sys/socket.h>         // For socket functions
#include <netinet/in.h>         // For sockaddr_in structure
#include <arpa/inet.h>          // For inet functions
#include <signal.h>             // For signal handling
#include "tcp.h"                // Custom header file

int Sockfd;                     // Global socket file descriptor for the server
int on;                         // For setsockopt

// Graceful shutdown of the server
void CloseServer() {
    close(Sockfd);              // Close the main socket
    printf("\nTCP Server exit.....\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int newSockfd;              // Socket descriptor for accepted connection
    int cliAddrLen, n;          // Client address length and bytes read
    struct sockaddr_in cliAddr, servAddr; // Structures for client and server addresses
    MsgType msg;                // Message structure from tcp.h
    pid_t pid;                  // Process ID for fork()

    // Signal handling
    signal(SIGINT, CloseServer);      // Handle Ctrl+C
    signal(SIGCHLD, SIG_IGN);         // Avoid zombie processes

    // Step 1: Create a TCP socket
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Step 2: Initialize server address structure
    int on = 1; // Allow the socket to be reused immediately after the server terminates
    if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections on any IP
    servAddr.sin_port = htons(SERV_TCP_PORT);     // Port number from tcp.h

    // Step 3: Bind the socket to the server address
    if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    // Step 4: Set the socket to listen for incoming connections
    listen(Sockfd, 5);
    printf("TCP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);

    // Main server loop: Accept and fork for each client
    while (1) {
        // Accept a new client connection
        newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
        if (newSockfd < 0) {
            perror("accept");
            continue; // Continue accepting other connections
        }

        // Fork a new process to handle the client
        pid = fork(); // Both parent and child point to the same file decsriptors
        if (pid < 0) {
            perror("fork");  // Print error if fork fails
            close(newSockfd);
            continue;
        }

        if (pid == 0) { // Child process
            close(Sockfd); // Close the main socket in the child process
            // Ensure that the child does not accidentally accept new connections

            // Step 5: Read the client's message
            if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0) {
                perror("read");
                close(newSockfd);
                exit(1);
            }
            printf("Received request: %s.....", msg.data);

            // Step 6: Prepare and send a reply
            msg.type = MSG_REPLY;
            sprintf(msg.data, "This is a reply from PID %d.", getpid());
            if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
                perror("write");
                close(newSockfd);
                exit(1);
            }
            printf("Replied.\n");

            close(newSockfd); // Close the client socket
            exit(0);          // Exit the child process
        } else { // Parent process
            close(newSockfd); // Parent doesn't need the client socket
        }
    }
}
