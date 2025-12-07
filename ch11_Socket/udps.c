#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "udp.h"


int	Sockfd;


// Function to gracefully close the server upon receiving a termination signal (e.g., Ctrl+C)
void
CloseServer()
{
	close(Sockfd);
	printf("\nUDP Server exit.....\n");

	exit(0);
}


main(int argc, char *argv[])
{
	int					cliAddrLen, n; // Client address length and bytes read
	struct sockaddr_in	cliAddr, servAddr; // Structures to hold client and server addresses
	MsgType				msg; // Message structure defined in udp.h
	int on; // For setsockopt

	signal(SIGINT, CloseServer); // Register signal handler for SIGINT (Ctrl+C)

	// Step 1: Create a UDP socket
	if ((Sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// Step 2: Initialize server address structure
	on = 1; // Allow the socket to be reused immediately after the server terminates
	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)  {
		perror("setsockopt");
		exit(1);
	}
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_UDP_PORT);

	// Step 3: Bind the socket to the server address
	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}

	printf("UDP Server started.....\n");

	// Step 4: Server main loop to receive and reply to client requests
	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		// Receive a request from a client
		if ((n = recvfrom(Sockfd, (char *)&msg, sizeof(msg), 
					0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  {
			perror("recvfrom");
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		// Prepare and send a reply to the client
		msg.type = MSG_REPLY;
		sprintf(msg.data, "This is a reply from %d.", getpid());
		if (sendto(Sockfd, (char *)&msg, sizeof(msg),
					0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
			perror("sendto");
			exit(1);
		}
		printf("Replied.\n");
	}
}