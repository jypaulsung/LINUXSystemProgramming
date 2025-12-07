#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "semlib2.h"

// Shared data and synchronization primitives
#define BUFFER_SIZE 256
char global_buffer[BUFFER_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
sem_t sem;

// Server thread function
void *server_thread(void *arg) {
    // Wait for request
    sem_wait(&sem); // Ensure the server runs only after the client sends a message

    // Access the shared global buffer
    pthread_mutex_lock(&mutex);
    printf("Server received: %s\n", global_buffer);

    // Write a reply
    sprintf(global_buffer, "This is a reply from the server.");
    pthread_cond_signal(&cond_var); // Notify the client
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Client thread function
void *client_thread(void *arg) {
    // Access the shared global buffer
    pthread_mutex_lock(&mutex);
    sprintf(global_buffer, "This is a request from the client.");
    printf("Client sent: %s\n", global_buffer);

    sem_post(&sem); // Signal the server that data is available
    pthread_cond_wait(&cond_var, &mutex); // Wait for the server's reply
    printf("Client received: %s\n", global_buffer);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t server, client;

    // Initialize the custom semaphore with a value of 0
    // Ensure the server waits for the client to signal before proceeding
    if (sem_init(&sem, 0, 0) != 0) {
        perror("sem_init");
        return 1;
    }

    // Create the client and server threads
    if (pthread_create(&server, NULL, server_thread, NULL) != 0) {
        perror("pthread_create (server)");
        return 1;
    }
    if (pthread_create(&client, NULL, client_thread, NULL) != 0) {
        perror("pthread_create (client)");
        return 1;
    }

    // Wait for both threads to finish
    pthread_join(client, NULL);
    pthread_join(server, NULL);

    // Clean up synchronization primitives
    sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}
