#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"     // Include semaphore library for synchronization
#include "prodcons.h"   // Include shared buffer structure and constants

main()
{
    BoundedBufferType *pBuf;    // Pointer to the shared memory buffer
    int shmid, i, data;         // Shared memory ID, loop counter, and data value
    int emptySemid, fullSemid, mutexSemid; // Semaphore IDs for empty, full, and mutex semaphores

    // Create or get the shared memory segment
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget"); // Handle error if shared memory creation fails
        exit(1);
    }
    // Attach the shared memory segment to the process address space
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1) {
        perror("shmat"); // Handle error if attaching shared memory fails
        exit(1);
    }

    // Initialize the empty semaphore to track available buffer slots
    if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    // Initialize the full semaphore to track filled buffer slots
    if ((fullSemid = semInit(FULL_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    // Initialize the mutex semaphore for mutual exclusion
    if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }

    srand(0x8888); // Seed the random number generator

    // Produce items in a loop
    for (i = 0; i < NLOOPS; i++) {
        // Wait for an empty slot in the buffer
        if (semWait(emptySemid) < 0) {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }
        // Wait for exclusive access to the buffer (mutex lock)
        if (semWait(mutexSemid) < 0) {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }

        // Critical Section: Add an item to the buffer
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;        // Generate random data to produce
        pBuf->buf[pBuf->in].data = data;      // Place data into the buffer at the 'in' index
        pBuf->in = (pBuf->in + 1) % MAX_BUF;  // Move 'in' index (circular buffer)
        pBuf->counter++;                      // Increment the buffer counter

        // Release exclusive access to the buffer (mutex unlock)
        if (semPost(mutexSemid) < 0) {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }
        // Signal that the buffer now has one more full slot
        if (semPost(fullSemid) < 0) {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }
        usleep(data); // Simulate production delay
    }

    // Print summary of production
    printf("Producer: Produced %d items.....\n", i);

    sleep(2); // Sleep to allow consumer to finish

    // Print the final buffer state
    printf("Producer: %d items in buffer.....\n", pBuf->counter);

    // Destroy semaphores
    if (semDestroy(emptySemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }
    if (semDestroy(fullSemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }
    if (semDestroy(mutexSemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }

    // Remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl"); // Handle error if shared memory deletion fails
        exit(1);
    }
}
