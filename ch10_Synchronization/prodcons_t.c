#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h" // Include header for buffer definition and constants

BoundedBufferType Buf; // Shared buffer structure used by producer and consumer

// Function to simulate a timed delay for a thread
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;         // Condition variable for timed wait
    pthread_mutex_t mutex;       // Mutex for synchronization
    struct timespec ts;          // Time specification for the wake-up time
    struct timeval tv;           // Structure to retrieve current time

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

    // Wait until the specified time
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

// Producer thread function: produces items and places them into the shared buffer
void Producer(void *dummy)
{
    int i, data; // Loop counter and data to produce

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Check if the buffer is full and wait until there is space
        if (Buf.counter == MAX_BUF) {
            printf("Producer: Buffer full. Waiting.....\n");
            while (Buf.counter == MAX_BUF) // Busy-wait for space to become available
                ;
        }

        // Produce an item
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;     // Generate random data
        Buf.buf[Buf.in].data = data;      // Add data to the buffer at 'in' index
        Buf.in = (Buf.in + 1) % MAX_BUF;  // Update 'in' index (circular buffer)
        Buf.counter++;                    // Increment buffer counter

        ThreadUsleep(data); // Simulate production time delay
    }

    // Print summary of produced items
    printf("Producer: Produced %d items.....\n", i);
    printf("Producer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Consumer thread function: consumes items from the shared buffer
void Consumer(void *dummy)
{
    int i, data; // Loop counter and data to consume

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        // Check if the buffer is empty and wait until there is an item to consume
        if (Buf.counter == 0) {
            printf("Consumer: Buffer empty. Waiting.....\n");
            while (Buf.counter == 0) // Busy-wait for data to become available
                ;
        }

        // Consume an item
        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;     // Retrieve data from 'out' index in the buffer
        Buf.out = (Buf.out + 1) % MAX_BUF; // Update 'out' index (circular buffer)
        Buf.counter--;                    // Decrement buffer counter

        ThreadUsleep((rand() % 100) * 10000); // Simulate consumption time delay
    }

    // Print summary of consumed items
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Main function: initializes threads and starts producer and consumer
main()
{
    pthread_t tid1, tid2; // Thread IDs for producer and consumer

    srand(0x8888); // Seed random number generator

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
}
