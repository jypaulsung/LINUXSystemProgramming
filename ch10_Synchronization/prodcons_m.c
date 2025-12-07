#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h" // Include shared buffer definition and constants

BoundedBufferType Buf;          // Shared buffer for producer and consumer
pthread_cond_t NotFull;          // Condition variable to signal buffer is not full
pthread_cond_t NotEmpty;         // Condition variable to signal buffer is not empty
pthread_mutex_t Mutex;           // Mutex to synchronize access to the shared buffer

// Function to simulate a timed delay for a thread
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;         // Condition variable for timing
    pthread_mutex_t mutex;       // Mutex for synchronization
    struct timespec ts;          // Time structure for the wake-up time
    struct timeval tv;           // To get the current time

    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000; // Calculate wake-up seconds
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000; // Calculate wake-up nanoseconds
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    pthread_cond_destroy(&cond); // Clean up condition variable
    pthread_mutex_destroy(&mutex); // Clean up mutex
}

// Producer thread function: adds items to the shared buffer
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (pthread_mutex_lock(&Mutex) < 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        // Wait until the buffer is not full
        while (Buf.counter == MAX_BUF) {
            if (pthread_cond_wait(&NotFull, &Mutex) < 0) {
                perror("pthread_cond_wait");
                pthread_exit(NULL);
            }
        }

        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000; // Generate random data
        Buf.buf[Buf.in].data = data;  // Place data in the buffer
        Buf.in = (Buf.in + 1) % MAX_BUF; // Update buffer's input index (circular buffer)
        Buf.counter++; // Increment the item counter

        // Signal that the buffer is not empty and unlock the mutex
        if (pthread_cond_signal(&NotEmpty) < 0) {
            perror("pthread_cond_signal");
            pthread_exit(NULL);
        }
        if (pthread_mutex_unlock(&Mutex) < 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }

        ThreadUsleep(data); // Simulate production delay
    }

    printf("Producer: Produced %d items.....\n", i);
    printf("Producer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Consumer thread function: removes items from the shared buffer
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (pthread_mutex_lock(&Mutex) < 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        // Wait until the buffer is not empty
        while (Buf.counter == 0) {
            if (pthread_cond_wait(&NotEmpty, &Mutex) < 0) {
                perror("pthread_cond_wait");
                pthread_exit(NULL);
            }
        }

        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data; // Retrieve data from the buffer
        Buf.out = (Buf.out + 1) % MAX_BUF; // Update buffer's output index (circular buffer)
        Buf.counter--; // Decrement the item counter

        // Signal that the buffer is not full and unlock the mutex
        if (pthread_cond_signal(&NotFull) < 0) {
            perror("pthread_cond_signal");
            pthread_exit(NULL);
        }
        if (pthread_mutex_unlock(&Mutex) < 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }

        ThreadUsleep((rand() % 100) * 10000); // Simulate consumption delay
    }

    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

// Main function: initializes synchronization primitives and starts threads
main()
{
    pthread_t tid1, tid2; // Thread identifiers for producer and consumer

    srand(0x8888); // Seed the random number generator

    // Initialize condition variables and mutex
    if (pthread_cond_init(&NotFull, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_cond_init(&NotEmpty, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&Mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    // Create producer and consumer threads
    if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }

    // Wait for both threads to complete
    if (pthread_join(tid1, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_join(tid2, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }

    printf("Main    : %d items in buffer.....\n", Buf.counter);

    // Destroy condition variables and mutex
    if (pthread_cond_destroy(&NotFull) < 0) {
        perror("pthread_cond_destroy");
    }
    if (pthread_cond_destroy(&NotEmpty) < 0) {
        perror("pthread_cond_destroy");
    }
    if (pthread_mutex_destroy(&Mutex) < 0) {
        perror("pthread_mutex_destroy");
    }
}
