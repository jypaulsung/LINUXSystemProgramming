#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chat.h"  // Custom header file for server configuration

#define MAX_BUF 256  // Maximum size for message buffer

int Sockfd;  // Socket file descriptor for client connection
pthread_t recvThread, sendThread; // Thread identifiers

// Function to handle server messages (Receiver Thread)
void *ReceiveMessages(void *arg) {
    char buf[MAX_BUF];
    int n;

    while (1) {
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
        fflush(stdout);
    }
    return NULL;
}

// Function to handle user input and send messages (Sender Thread)
void *SendMessages(void *arg) {
    char buf[MAX_BUF];
    int n;

    while (1) {
        fgets(buf, MAX_BUF, stdin);  // Read input from user
        if ((n = send(Sockfd, buf, strlen(buf) + 1, 0)) < 0) {
            perror("send");
            exit(1);
        }
    }
    return NULL;
}

// Handles the termination of the client upon receiving a signal (e.g., SIGINT)
void CloseClient(int signo) {
    close(Sockfd);  // Close the client socket
    printf("\nChat client terminated.....\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in servAddr;  // Server address structure
    struct hostent *hp;           // Host entity for resolving server address
    char buf[MAX_BUF];

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
    } else {  // If the input is a hostname
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

    // Prompt user for their ID
    printf("Enter ID: ");
    fflush(stdout);
    fgets(buf, MAX_BUF, stdin);
    *strchr(buf, '\n') = '\0';  // Replace newline character with null terminator

    // Send user ID to the server
    if (send(Sockfd, buf, strlen(buf) + 1, 0) < 0) {
        perror("send");
        exit(1);
    }

    printf("Press ^C to exit\n");

    // Create threads for receiving and sending messages
    if (pthread_create(&recvThread, NULL, ReceiveMessages, NULL) != 0) {
        perror("pthread_create (recvThread)");
        exit(1);
    }
    if (pthread_create(&sendThread, NULL, SendMessages, NULL) != 0) {
        perror("pthread_create (sendThread)");
        exit(1);
    }

    // Join threads (optional: blocks until threads finish)
    pthread_join(recvThread, NULL);
    pthread_join(sendThread, NULL);

    return 0;
}
