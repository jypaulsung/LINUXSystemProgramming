#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "select.h"

int TcpSockfd;
int UdpSockfd;
int UcoSockfd;
int UclSockfd;
int on; // for setsockopt

// Function to close all sockets and clean up before exiting
void CloseServer()
{
    close(TcpSockfd);
    close(UdpSockfd);
    close(UcoSockfd);
    close(UclSockfd);
    
    // Remove UNIX socket files
    if (remove(UNIX_STR_PATH) < 0) {
        perror("remove");
    }
    if (remove(UNIX_DG_PATH) < 0) {
        perror("remove");
    }

    printf("\nServer daemon exit.....\n");
    exit(0);
}

// Create and initialize a TCP socket
void MakeTcpSocket()
{
    struct sockaddr_in servAddr;

    if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

	// Allow the socket to be reused immediately after the server terminates
	on = 1;
	if (setsockopt(TcpSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

    // Set up server address structure
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;            // Internet family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_TCP_PORT); // TCP port

    // Bind the socket to the address and port
    if (bind(TcpSockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    // Set the socket to listen for connections
    listen(TcpSockfd, 5);
}

// Create and initialize a UDP socket
void MakeUdpSocket()
{
    struct sockaddr_in servAddr;

    if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

	// Allow the socket to be reused immediately after the server terminates
	on = 1;
	if (setsockopt(TcpSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

    // Set up server address structure
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_UDP_PORT);

    // Bind the socket
    if (bind(UdpSockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }
}

// Create and initialize a UNIX-domain stream socket
void MakeUcoSocket()
{
    struct sockaddr_un servAddr;
    int servAddrLen;

    if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

	// Allow the socket to be reused immediately after the server terminates
	on = 1;
	if (setsockopt(TcpSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

    // Set up server address for UNIX socket
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_STR_PATH);
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    // Bind and listen on UNIX stream socket
    if (bind(UcoSockfd, (struct sockaddr *)&servAddr, servAddrLen) < 0) {
        perror("bind");
        exit(1);
    }
    listen(UcoSockfd, 5);
}

// Create and initialize a UNIX-domain datagram socket
void MakeUclSocket()
{
    struct sockaddr_un servAddr;
    int servAddrLen;

    if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

	// Allow the socket to be reused immediately after the server terminates
	on = 1;
	if (setsockopt(TcpSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_DG_PATH);
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    if (bind(UclSockfd, (struct sockaddr *)&servAddr, servAddrLen) < 0) {
        perror("bind");
        exit(1);
    }
}

// Process incoming TCP connections
void ProcessTcpRequest()
{
    int newSockfd, cliAddrLen, n;
    struct sockaddr_in cliAddr;
    MsgType msg;

    cliAddrLen = sizeof(cliAddr);
    newSockfd = accept(TcpSockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);

    if (newSockfd < 0) {
        perror("accept");
        exit(1);
    }

    // Read the request message
    if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0) {
        perror("read");
        exit(1);
    }

    printf("Received TCP request: %s.....", msg.data);

    // Send a reply message
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
        perror("write");
        exit(1);
    }
    printf("Replied.\n");

    close(newSockfd);
}

// Process incoming UDP messages
void ProcessUdpRequest()
{
    int cliAddrLen, n;
    struct sockaddr_in cliAddr;
    MsgType msg;

    cliAddrLen = sizeof(cliAddr);

    // Receive a UDP request
    if ((n = recvfrom(UdpSockfd, (char *)&msg, sizeof(msg), 0,
                      (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0) {
        perror("recvfrom");
        exit(1);
    }

    printf("Received UDP request: %s.....", msg.data);

    // Send a reply
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    if (sendto(UdpSockfd, (char *)&msg, sizeof(msg), 0,
               (struct sockaddr *)&cliAddr, cliAddrLen) < 0) {
        perror("sendto");
        exit(1);
    }

    printf("Replied.\n");
}

void ProcessUcoRequest() 
{ /* Similar to ProcessTcpRequest for UNIX socket */ 
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_STR_PATH);
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	if (bind(UcoSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	listen(UcoSockfd, 5);
}
void ProcessUclRequest() 
{ /* Similar to ProcessUdpRequest for UNIX datagram socket */ 
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_DG_PATH);
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	if (bind(UclSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}
}

// Main server loop
int main(int argc, char *argv[]) 
{
    fd_set fdvar; // File descriptor set for select
    int count;
    pthread_t tid;

    signal(SIGINT, CloseServer);
    MakeTcpSocket();
    MakeUdpSocket();
    MakeUcoSocket();
    MakeUclSocket();

    printf("Server daemon started.....\n");

    while (1) {
        // Reset file descriptor set
        FD_ZERO(&fdvar);
        FD_SET(TcpSockfd, &fdvar);
        FD_SET(UdpSockfd, &fdvar);
        FD_SET(UcoSockfd, &fdvar);
        FD_SET(UclSockfd, &fdvar);

        // Monitor sockets using select
        if ((count = select(FD_SETSIZE, &fdvar, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(1);
        }

        // Check which sockets are ready
        // pthread_detach is used to allow the main thread to continue monitoring without blocking
        while (count--) {
            if (FD_ISSET(TcpSockfd, &fdvar)) {
                pthread_create(&tid, NULL, (void *(*)(void *))ProcessTcpRequest, NULL);
                pthread_detach(tid);
            } 
            else if (FD_ISSET(UdpSockfd, &fdvar)) {
                pthread_create(&tid, NULL, (void *(*)(void *))ProcessUdpRequest, NULL);
                pthread_detach(tid);
            } 
            else if (FD_ISSET(UcoSockfd, &fdvar)) {
                pthread_create(&tid, NULL, (void *(*)(void *))ProcessUcoRequest, NULL);
                pthread_detach(tid);
            } 
            else if (FD_ISSET(UclSockfd, &fdvar)) {
                pthread_create(&tid, NULL, (void *(*)(void *))ProcessUclRequest, NULL);
                pthread_detach(tid);
            }
        }
    }
    return 0;
}
