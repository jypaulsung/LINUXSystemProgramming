#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h" // Include buffer definition and constants

// Shared buffer structure for producer-consumer
BoundedBufferType Buf;

// Semaphores for synchronization
sem_t EmptySem;  // Tracks empty slots in the buffer
sem_t FullSem;   // Tracks full slots in the buffer
sem_t MutexSem;  // Mutex for mutual exclusion on the buffer

// Function to simulate a thread sleeping for a specified time
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;         // Condition variable for timed wait
    pthread_mutex_t mutex;       // Mutex for synchronization
    struct timespec ts;          // Time specification for wake-up time
    struct timeval tv;           // Current time retrieval

    // Initialize condition variable and mutex
    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    // Calculate wake-up time
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    // Lock mutex and wait until the specified time
    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    // Destroy condition variable and mutex after use
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

// Producer thread function: produces items and places them into the buffer
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Wait for an empty slot (P operation on EmptySem)
        if (sem_wait(&EmptySem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        // Wait for mutual exclusion (P operation on MutexSem)
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        // Critical section: Add data to the buffer
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;     // Generate random data
        Buf.buf[Buf.in].data = data;       // Add data at the 'in' position
        Buf.in = (Buf.in + 1) % MAX_BUF;   // Update 'in' index (circular buffer)
        Buf.counter++;                     // Increment buffer counter

        // Release mutual exclusion (V operation on MutexSem)
        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        // Signal that a slot is full (V operation on FullSem)
        if (sem_post(&FullSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }

        ThreadUsleep(data); // Simulate production delay
    }

    // Print summary of produced items
    printf("Producer: Produced %d items.....\n", i);
    printf("Producer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Consumer thread function: consumes items from the buffer
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Wait for a full slot (P operation on FullSem)
        if (sem_wait(&FullSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        // Wait for mutual exclusion (P operation on MutexSem)
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        // Critical section: Remove data from the buffer
        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;      // Retrieve data at the 'out' position
        Buf.out = (Buf.out + 1) % MAX_BUF; // Update 'out' index (circular buffer)
        Buf.counter--;                     // Decrement buffer counter

        // Release mutual exclusion (V operation on MutexSem)
        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        // Signal that a slot is empty (V operation on EmptySem)
        if (sem_post(&EmptySem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }

        ThreadUsleep((rand() % 100) * 10000); // Simulate consumption delay
    }

    // Print summary of consumed items
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Main function: initializes semaphores, creates threads, and cleans up
main()
{
    pthread_t tid1, tid2; // Thread IDs for producer and consumer

    srand(0x9999); // Seed random number generator

    // Initialize semaphores
    if (sem_init(&EmptySem, 0, MAX_BUF) < 0) { // Start with buffer fully empty
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&FullSem, 0, 0) < 0) { // Start with no full slots
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&MutexSem, 0, 1) < 0) { // Mutex initialized to 1
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

    // Wait for producer thread to finish
    if (pthread_join(tid1, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }
    // Wait for consumer thread to finish
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