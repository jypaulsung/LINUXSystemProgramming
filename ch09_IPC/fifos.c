#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "fifo.h" // Include header for FIFO constants and message structure

// Signal handler for SIGINT (Ctrl+C)
void SigIntHandler(int signo)
{
    // Remove the server FIFO when the server is interrupted
    if (remove(SERV_FIFO) < 0) {
        perror("remove"); // Print error if FIFO removal fails
        exit(1);
    }

    exit(0); // Exit the program cleanly
}

main()
{
    int fd, cfd, n;     // File descriptors and a variable for read/write operations
    MsgType msg;        // Message structure to store request and response

    // Set up signal handler for SIGINT to clean up the FIFO upon termination
    if (signal(SIGINT, SigIntHandler) == SIG_ERR) {
        perror("signal"); // Print error if signal setup fails
        exit(1);
    }

    // Create the server FIFO (named pipe)
    if (mkfifo(SERV_FIFO, 0600) < 0) {
        if (errno != EEXIST) { // If the FIFO already exists, skip creating it
            perror("mkfifo"); // Print error if FIFO creation fails
            exit(1);
        }
    }

    // Open the server FIFO for reading and writing
    if ((fd = open(SERV_FIFO, O_RDWR)) < 0) {
        perror("open"); // Print error if opening FIFO fails
        exit(1);
    }

    // Server loop to continuously handle client requests
    while (1) {
        // Read the message from the server FIFO
        if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0) {
            if (errno == EINTR) { // If interrupted by a signal, continue
                continue;
            } else {
                perror("read"); // Print error if read fails
                exit(1);
            }
        }

        // Print the received request
        printf("Received request: %s.....", msg.data);

        // Open the client FIFO specified in the message for writing
        if ((cfd = open(msg.returnFifo, O_WRONLY)) < 0) {
            perror("open"); // Print error if opening client FIFO fails
            exit(1);
        }

        // Prepare the response message with the server's process ID
        sprintf(msg.data, "This is a reply from %d.", getpid());

        // Send the response message to the client
        write(cfd, (char *)&msg, sizeof(msg));
        close(cfd); // Close the client FIFO file descriptor
        printf("Replied.\n"); // Confirm the response was sent
    }
}
