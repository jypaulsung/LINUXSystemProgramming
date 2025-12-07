#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp.h"


main(int argc, char *argv[])
{
	int					sockfd, n, peerAddrLen; // Socket file descriptor, number of bytes read, and length of peer address
	struct sockaddr_in	servAddr, peerAddr;     // Structure to hold server and peer address information
	MsgType				msg; 			     	// Message structure defined in udp.h to store data to send/receive

	// Step 1: Create a UDP socket
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// Step 2: Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	servAddr.sin_port = htons(SERV_UDP_PORT);

	// Step 3: Prepare a request message to send to the server
	msg.type = MSG_REQUEST;
	sprintf(msg.data, "This is a request from %d.", getpid());

	// Step 4: Send the request message to the server
	if (sendto(sockfd, (char *)&msg, sizeof(msg), 
			0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)  {
		perror("sendto");
		exit(1);
	}
	printf("Sent a request.....");
	
	// Step 5: Read the reply from the server
	peerAddrLen = sizeof(peerAddr);
	if ((n = recvfrom(sockfd, (char *)&msg, sizeof(msg),
				0, (struct sockaddr *)&peerAddr, &peerAddrLen)) < 0)  {
		perror("recvfrom");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	// Step 6: Close the socket and terminate
	close(sockfd);
}