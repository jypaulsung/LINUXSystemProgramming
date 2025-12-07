#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "fifo.h" // Include header for message structure and FIFO constants

pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for synchronizing write operations

main()
{
    char fname[MAX_FIFO_NAME]; // Buffer to store the name of the FIFO
    int cfd, sfd, n;           // File descriptors and a variable for read/write operations
    MsgType msg;              // Message structure to send and receive data

    // Create a unique FIFO name for the client using its process ID
    sprintf(fname, ".fifo%d", getpid());

    // Create a FIFO (named pipe) with read/write permissions for the owner
    if (mkfifo(fname, 0600) < 0) {
        perror("mkfifo"); // Print error if FIFO creation fails
        exit(1);
    }

    // Open the client FIFO for reading and writing
    if ((cfd = open(fname, O_RDWR)) < 0) {
        perror("open"); // Print error if FIFO open fails
        exit(1);
    }

    // Open the server FIFO to send the request
    if ((sfd = open(SERV_FIFO, O_RDWR)) < 0) {
        perror("open"); // Print error if opening server FIFO fails
        exit(1);
    }

    // Prepare the message to send to the server
    strcpy(msg.returnFifo, fname); // Set the return FIFO name in the message
    sprintf(msg.data, "This is a request from %d.", getpid()); // Add request data with process ID

    // Synchronize the write operation using a mutex
    pthread_mutex_lock(&write_mutex); // Lock the mutex before writing to the server FIFO
    write(sfd, (char *)&msg, sizeof(msg));
    pthread_mutex_unlock(&write_mutex); // Unlock the mutex after writing

    printf("Sent a request.....");

    // Wait for and read the server's reply from the client FIFO
    if ((n = read(cfd, (char *)&msg, sizeof(msg))) < 0) {
        perror("read"); // Print error if reading from FIFO fails
        exit(1);
    }

    // Print the server's reply message
    printf("Received reply: %s\n", msg.data);

    // Close the file descriptors for both FIFOs
    close(cfd);
    close(sfd);

    // Remove the client FIFO to clean up
    if (remove(fname) < 0) {
        perror("remove"); // Print error if removing the FIFO fails
        exit(1);
    }

    pthread_mutex_destroy(&write_mutex); // Destroy the mutex
}
