#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include "fifo.h" // Include header for FIFO constants and message structure

#define MAX_THREADS 5 // Maximum number of concurrent threads

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for mutual exclusion
sem_t thread_sem; // Semaphore to control the number of active threads

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

// Thread function to handle a client request
void *HandleClient(void *arg)
{
    MsgType msg = *(MsgType *)arg; // Copy the message
    int cfd;

    // Open the client FIFO for writing
    if ((cfd = open(msg.returnFifo, O_WRONLY)) < 0) {
        perror("open"); // Print error if opening client FIFO fails
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&mutex); // Lock to safely print output
    printf("Received request: %s\n", msg.data);
    pthread_mutex_unlock(&mutex);

    // Prepare the response message with the server's process ID
    sprintf(msg.data, "This is a reply from %d.", getpid());

    // Send the response message to the client
    if (write(cfd, (char *)&msg, sizeof(msg)) < 0) {
        perror("write");
        close(cfd);
        pthread_exit(NULL);
    }
    close(cfd); // Close the client FIFO file descriptor

    pthread_mutex_lock(&mutex); // Lock to safely print output
    printf("Replied.\n"); // Confirm the response was sent
    pthread_mutex_unlock(&mutex);

    sem_post(&thread_sem); // Signal that this thread has finished
    pthread_exit(NULL);
}

main()
{
    int fd, n;
    MsgType msg;
    pthread_t tid;

    // Set up signal handler for SIGINT to clean up the FIFO upon termination
    if (signal(SIGINT, SigIntHandler) == SIG_ERR) {
        perror("signal"); // Print error if signal setup fails
        exit(1);
    }

    // Initialize semaphore to allow a limited number of concurrent threads
    if (sem_init(&thread_sem, 0, MAX_THREADS) < 0) {
        perror("sem_init");
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

    printf("Server started. Waiting for requests...\n");

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

        // Wait if the maximum number of threads is reached
        sem_wait(&thread_sem);

        // Create a thread to handle the client request
        if (pthread_create(&tid, NULL, HandleClient, (void *)&msg) < 0) {
            perror("pthread_create");
            sem_post(&thread_sem);
            continue;
        }

        pthread_detach(tid); // Detach the thread to allow automatic cleanup
    }

    // Clean up resources (this part will execute only if the loop ends)
    sem_destroy(&thread_sem);
    pthread_mutex_destroy(&mutex);
    close(fd);
    if (remove(SERV_FIFO) < 0) {
        perror("remove");
    }
    exit(0);
}
