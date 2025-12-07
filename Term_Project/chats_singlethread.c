#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "chat.h"

#define DEBUG           // Enables debugging messages during execution

#define MAX_CLIENT 5
#define MAX_ID 32
#define MAX_BUF 256

typedef struct {
    int sockfd;            // Client socket descriptor
    int inUse;             // Flag to mark active clients
    char uid[MAX_ID];      // Client ID
} ClientType;

int Sockfd;               // Server socket
ClientType Client[MAX_CLIENT];  // Array to manage clients

// Handle CTRL+C to terminate the server
void CloseServer(int signo) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse) {
            close(Client[i].sockfd);
            Client[i].inUse = 0;
        }
    }
    close(Sockfd);
    printf("\nChat server terminated.....\n");
    exit(0);
}

// Broadcast message to other clients
void SendToOtherClients(int id, char *buf) {
    char msg[MAX_BUF + MAX_ID];
    sprintf(msg, "%s> %s", Client[id].uid, buf);
#ifdef	DEBUG
	printf("%s", msg);   // Debug: Print message to server console
	fflush(stdout);
#endif

    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse && i != id) {
            if (send(Client[i].sockfd, msg, strlen(msg) + 1, 0) < 0) {
                perror("send");
                close(Client[i].sockfd);
                Client[i].inUse = 0;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int maxfd, newSockfd, cliAddrLen, n;
    struct sockaddr_in cliAddr, servAddr;
    fd_set rset, allset;

    signal(SIGINT, CloseServer);

    // Create server socket
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Allow address reuse
    int one = 1;
    setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    // Set server address
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_TCP_PORT);

    // Bind socket to the server address
    if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    listen(Sockfd, MAX_CLIENT);
    printf("Chat server started.....\n");

    // Initialize client array
    for (int i = 0; i < MAX_CLIENT; i++) Client[i].inUse = 0;

    FD_ZERO(&allset);
    FD_SET(Sockfd, &allset);
    maxfd = Sockfd;

    while (1) {
        rset = allset; // Reset monitored descriptors

        if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        // New connection request
        if (FD_ISSET(Sockfd, &rset)) {
            cliAddrLen = sizeof(cliAddr);
            if ((newSockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0) {
                perror("accept");
                continue;
            }

            int id = -1;
            for (int i = 0; i < MAX_CLIENT; i++) {
                if (!Client[i].inUse) {
                    id = i;
                    break;
                }
            }

            if (id >= 0) {
                Client[id].sockfd = newSockfd;
                Client[id].inUse = 1;
                FD_SET(newSockfd, &allset);
                if (newSockfd > maxfd) maxfd = newSockfd;

                // Receive client ID
                if ((n = recv(newSockfd, Client[id].uid, MAX_ID, 0)) > 0) {
                    printf("Client %d log-in(ID: %s).....\n", id, Client[id].uid);
                } else {
                    close(newSockfd);
                    Client[id].inUse = 0;
                }
            } else {
                printf("Max clients reached. Rejecting connection.\n");
                close(newSockfd);
            }
        }

        // Check all clients for incoming messages
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (Client[i].inUse && FD_ISSET(Client[i].sockfd, &rset)) {
                char buf[MAX_BUF];
                if ((n = recv(Client[i].sockfd, buf, MAX_BUF, 0)) > 0) {
                    SendToOtherClients(i, buf);
                } else {  // Client disconnected
                    printf("Client %d log-out(ID: %s).....\n", i, Client[i].uid);
                    close(Client[i].sockfd);
                    FD_CLR(Client[i].sockfd, &allset);
                    Client[i].inUse = 0;

                    strcpy(buf, "log-out.....\n");
                    SendToOtherClients(i, buf);
                }
            }
        }
    }
}
