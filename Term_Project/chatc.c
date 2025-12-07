#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chat.h"  // Custom header file for server configuration

#define MAX_BUF 256  // Maximum size for message buffer

int Sockfd;  // Socket file descriptor for client connection

// Manages communication between the client and the server
// Handles both sending and receiving messages using `select` to monitor sockets
void ChatClient(void)
{
    char buf[MAX_BUF];  // Buffer to store messages
    int count, n;       // Variables to store count of descriptors and message size
    fd_set fdset;       // File descriptor set for select

    // Prompt the user to enter their ID
    printf("Enter ID: ");
    fflush(stdout);
    fgets(buf, MAX_BUF, stdin);  // Get user ID input
    *strchr(buf, '\n') = '\0';  // Replace newline character with null terminator

    // Send the user ID to the server
    if (send(Sockfd, buf, strlen(buf) + 1, 0) < 0) {
        perror("send");
        exit(1);
    }
    printf("Press ^C to exit\n");

    while (1) {
        FD_ZERO(&fdset);             // Clear the file descriptor set
        FD_SET(Sockfd, &fdset);      // Add server socket to the set
        FD_SET(STDIN_FILENO, &fdset); // Add standard input to the set

        // Use select to monitor server socket and user input
        if ((count = select(10, &fdset, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(1);
        }

        while (count--) {  // Handle all active descriptors
            if (FD_ISSET(Sockfd, &fdset)) {  // Check if server sent a message
                if ((n = recv(Sockfd, buf, MAX_BUF, 0)) < 0) {
                    perror("recv");
                    exit(1);
                }
                if (n == 0) {  // Server disconnected
                    fprintf(stderr, "Server terminated.....\n");
                    close(Sockfd);
                    exit(1);
                }
                printf("%s", buf);  // Print message from server
            }
            else if (FD_ISSET(STDIN_FILENO, &fdset)) {  // Check if user entered input
                fgets(buf, MAX_BUF, stdin);  // Read input from user
                if ((n = send(Sockfd, buf, strlen(buf) + 1, 0)) < 0) {
                    perror("send");
                    exit(1);
                }
            }
        }
    }
}

// Handles the termination of the client upon receiving a signal (e.g., SIGINT)
// Closes the client socket and exits the program
void CloseClient(int signo)
{
    close(Sockfd);  // Close the client socket
    printf("\nChat client terminated.....\n");
    exit(0);
}

// Establishes a connection to the chat server and starts the client process
main(int argc, char *argv[])
{
    struct sockaddr_in servAddr;  // Server address structure
    struct hostent *hp;           // Host entity for resolving server address

    // Check for correct command-line usage
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
        exit(1);
    }

    // Create a socket for the client
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Set server address properties
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(SERV_TCP_PORT);  // Server port from chat.h

    // Resolve server IP address
    if (isdigit(argv[1][0])) {  // If the input is an IP address
        servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else {  // If the input is a hostname
        if ((hp = gethostbyname(argv[1])) == NULL) {
            fprintf(stderr, "Unknown host: %s\n", argv[1]);
            exit(1);
        }
        memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
    }

    // Connect to the server
    if (connect(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        exit(1);
    }

    // Register signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, CloseClient);

    // Start the chat client process
    ChatClient();
}
