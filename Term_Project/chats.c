#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "chat.h"

#define	DEBUG           // Enables debugging messages during execution

#define	MAX_CLIENT	5   // Maximum number of concurrent clients
#define	MAX_ID		32  // Maximum length of a client ID
#define	MAX_BUF		256 // Maximum size of a message buffer

// Structure to hold client information
typedef	struct  {
	int			sockfd;     // Socket descriptor for client connection
	int			inUse;      // Flag indicating if the slot is in use
	pthread_t	tid;        // Thread ID for the client handler
	char		uid[MAX_ID];// User ID for the client
} ClientType;

int				Sockfd;      // Server socket file descriptor
pthread_mutex_t	Mutex;       // Mutex to protect shared resources

ClientType		Client[MAX_CLIENT];  // Array to store client information

// Finds an available slot in the Client array for a new connection
// Returns the index of the available slot or exits if none is available
int
GetID()
{
	int	i;

	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (! Client[i].inUse)  {  // Check if slot is free
			Client[i].inUse = 1;   // Mark slot as in use
			return i;
		}
	}
}

// Broadcast received message to all other connected clients
void
SendToOtherClients(int id, char *buf) //(index of the client sending the message, message content to broadcast)
{
	int		i;
	char	msg[MAX_BUF+MAX_ID];  // Buffer for message with client ID

	sprintf(msg, "%s> %s", Client[id].uid, buf);  // Format message with sender's ID
#ifdef	DEBUG
	printf("%s", msg);   // Debug: Print message to server console
	fflush(stdout);
#endif

	pthread_mutex_lock(&Mutex);  // Lock mutex before accessing shared resource
	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (Client[i].inUse && (i != id))  {  // Exclude the sender
			if (send(Client[i].sockfd, msg, strlen(msg)+1, 0) < 0)  {
				perror("send");
				exit(1);
			}
		}
	}
	pthread_mutex_unlock(&Mutex);  // Unlock mutex
}
	
// Handles communication with a connected client in a separate thread
void
ProcessClient(int id) // (index of the client in the Client array)
{
	char	buf[MAX_BUF];
	int		n;

	// Set thread cancellation properties
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
		perror("pthread_setcancelstate");
		exit(1);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
		perror("pthread_setcanceltype");
		exit(1);
	}

	// Receive user ID from the client
	if ((n = recv(Client[id].sockfd, Client[id].uid, MAX_ID, 0)) < 0)  {
		perror("recv");
		exit(1);
	}
	printf("Client %d log-in(ID: %s).....\n", id, Client[id].uid);

	// Continuously receive and broadcast messages
	while (1)  {
		if ((n = recv(Client[id].sockfd, buf, MAX_BUF, 0)) < 0)  {
			perror("recv");
			exit(1);
		}
		if (n == 0)  {  // Client disconnected
			printf("Client %d log-out(ID: %s).....\n", id, Client[id].uid);

			pthread_mutex_lock(&Mutex);
			close(Client[id].sockfd);
			Client[id].inUse = 0;
			pthread_mutex_unlock(&Mutex);

			strcpy(buf, "log-out.....\n");
			SendToOtherClients(id, buf);  // Notify other clients

			pthread_exit(NULL);
		}

		SendToOtherClients(id, buf);  // Broadcast message to other clients
	}
}

// Gracefully shuts down the server, canceling client threads and closing sockets
void
CloseServer(int signo) // parameters: Signal number (unused)
{
	int		i;

	close(Sockfd);  // Close server socket

	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (Client[i].inUse)  {  // Cancel threads for active clients
			if (pthread_cancel(Client[i].tid))  {
				perror("pthread_cancel");
				exit(1);
			}
			if (pthread_join(Client[i].tid, NULL))  {
				perror("pthread_join");
				exit(1);
			}
			close(Client[i].sockfd);
		}
	}
	if (pthread_mutex_destroy(&Mutex) < 0)  {  // Destroy mutex
		perror("pthread_mutex_destroy");
		exit(1);
	}

	printf("\nChat server terminated.....\n");
	exit(0);
}

main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, id, one = 1;
	struct sockaddr_in	cliAddr, servAddr;

	signal(SIGINT, CloseServer);  // Handle CTRL+C to terminate server
	if (pthread_mutex_init(&Mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		exit(1);
	}

	// Create server socket
	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// Allow address reuse
	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)  {
		perror("setsockopt");
		exit(1);
	}

	// Set server address
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_TCP_PORT);

	// Bind socket to the server address
	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5);  // Listen for incoming connections

	printf("Chat server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}

		id = GetID();  // Get an available slot for the client
		Client[id].sockfd = newSockfd;

		// Create a new thread to handle the client
		if (pthread_create(&Client[id].tid, NULL, (void *)ProcessClient, (void *)id) < 0)  {
			perror("pthread_create");
			exit(1);
		}
	}
}
