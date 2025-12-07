#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "semlib2.h"   // Include semaphore library
#include "prodcons.h"  // Include buffer definition and constants

BoundedBufferType Buf;      // Shared buffer for producer and consumer
sem_t EmptySem, FullSem;    // Semaphores for empty and full slots
sem_t MutexSem;             // Semaphore for mutual exclusion (mutex)

// Function to simulate a timed delay (sleep) for a thread
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;         // Condition variable for timed sleep
    pthread_mutex_t mutex;       // Mutex for synchronization
    struct timespec ts;          // Time specification for the wake-up time
    struct timeval tv;           // To get the current system time

    // Initialize condition variable and mutex
    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    // Calculate the wake-up time
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    // Sleep until the specified time
    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    // Clean up condition variable and mutex
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

// Producer thread function: produces items and places them into the shared buffer
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Wait until there is an empty slot (P operation on EmptySem)
        if (sem_wait(&EmptySem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        // Wait for mutual exclusion (lock the buffer)
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        // Critical Section: Produce an item
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;      // Generate a random item
        Buf.buf[Buf.in].data = data;       // Place the item in the buffer
        Buf.in = (Buf.in + 1) % MAX_BUF;   // Update the 'in' index (circular buffer)
        Buf.counter++;                     // Increment the buffer counter

        // Release mutual exclusion (unlock the buffer)
        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        // Signal that a slot is full (V operation on FullSem)
        if (sem_post(&FullSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }

        // Simulate production time delay
        ThreadUsleep(data);
    }

    // Print producer summary
    printf("Producer: Produced %d items.....\n", i);
    printf("Producer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Consumer thread function: consumes items from the shared buffer
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Wait until there is a full slot (P operation on FullSem)
        if (sem_wait(&FullSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        // Wait for mutual exclusion (lock the buffer)
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        // Critical Section: Consume an item
        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;      // Retrieve the item from the buffer
        Buf.out = (Buf.out + 1) % MAX_BUF; // Update the 'out' index (circular buffer)
        Buf.counter--;                     // Decrement the buffer counter

        // Release mutual exclusion (unlock the buffer)
        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        // Signal that a slot is empty (V operation on EmptySem)
        if (sem_post(&EmptySem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }

        // Simulate consumption time delay
        ThreadUsleep((rand() % 100) * 10000);
    }

    // Print consumer summary
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Main function: initializes semaphores, creates threads, and cleans up
main()
{
    pthread_t tid1, tid2; // Thread IDs for producer and consumer

    srand(0x8888); // Seed random number generator

    // Initialize semaphores
    if (sem_init(&EmptySem, 0, MAX_BUF) < 0) {
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&FullSem, 0, 0) < 0) {
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&MutexSem, 0, 1) < 0) {
        perror("sem_init");
        exit(1);
    }

    // Create producer thread
    if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }
    // Create consumer thread
    if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }

    // Wait for threads to finish
    if (pthread_join(tid1, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_join(tid2, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }

    // Print final buffer state
    printf("Main    : %d items in buffer.....\n", Buf.counter);

    // Destroy semaphores
    if (sem_destroy(&EmptySem) < 0) {
        perror("sem_destroy");
    }
    if (sem_destroy(&FullSem) < 0) {
        perror("sem_destroy");
    }
    if (sem_destroy(&MutexSem) < 0) {
        perror("sem_destroy");
    }
}
