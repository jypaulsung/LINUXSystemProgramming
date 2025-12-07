#include <stdio.h>  // Standard I/O functions
#include <string.h> // String manipulation functions
#include <sys/types.h> // Definitions of data types used in system calls
#include <sys/socket.h> // Socket programming functions and structures
#include <netinet/in.h> // Internet address family structures
#include <arpa/inet.h> // Functions for Internet operations
#include <netdb.h> // Hostname resolution functions
#include "tcp.h" // Custom header file with constants and types

main(int argc, char *argv[]) {
    int sockfd, n; // Socket file descriptor and a variable for received data size
    struct sockaddr_in servAddr; // Structure to store server address
    MsgType msg; // Message structure defined in tcp.h
    struct hostent *hp; // Host entry structure for DNS resolution

    // Check if the correct number of arguments is passed
    if (argc != 2) {
        fprintf(stderr, "Usage: %s IPaddress\n", argv[0]); // Print usage message
        exit(1); // Exit if incorrect usage
    }

    // Create a TCP socket
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); // Print error message if socket creation fails
        exit(1);
    }

    // Initialize the server address structure to zero
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET; // Set the address family to Internet
    servAddr.sin_port = htons(SERV_TCP_PORT); // Set the server port number (defined in tcp.h)

    // Determine if the input is an IP address or a hostname
    if (isdigit(argv[1][0])) { // Check if the first character of the input is a digit
        servAddr.sin_addr.s_addr = inet_addr(argv[1]); // Convert IP address to binary form
    } else {
        // Perform DNS lookup to resolve hostname to IP address
        if ((hp = gethostbyname(argv[1])) == NULL) {
            fprintf(stderr, "Unknown host: %s\n", argv[1]); // Print error for unknown host
            exit(1); // Exit if DNS lookup fails
        }
        memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length); // Copy the resolved IP address
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect"); // Print error message if connection fails
        exit(1);
    }

    // Prepare and send a request message
    msg.type = MSG_REQUEST; // Set message type to request
    sprintf(msg.data, "This is a request from %d.", getpid()); // Add process ID to the message
    if (write(sockfd, (char *)&msg, sizeof(msg)) < 0) {
        perror("write"); // Print error message if sending fails
        exit(1);
    }
    printf("Sent a request.....\n"); // Indicate request has been sent

    // Receive the reply from the server
    if ((n = read(sockfd, (char *)&msg, sizeof(msg))) < 0) {
        perror("read"); // Print error message if receiving fails
        exit(1);
    }
    printf("Received reply: %s\n", msg.data); // Print the received reply

    // Close the socket
    close(sockfd);
}
