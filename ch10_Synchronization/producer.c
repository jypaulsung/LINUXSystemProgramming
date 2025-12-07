#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h" // Include shared buffer definition and constants

main()
{
    BoundedBufferType *pBuf;    // Pointer to the shared memory buffer
    int shmid, i, data;         // Shared memory ID, loop counter, and data to produce

    // Create or get the shared memory segment
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget"); // Handle error if shared memory creation fails
        exit(1);
    }

    // Attach the shared memory segment to the process address space
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat"); // Handle error if attaching shared memory fails
        exit(1);
    }

    srand(0x8888); // Seed the random number generator

    // Produce items in a loop
    for (i = 0; i < NLOOPS; i++) {
        // Check if the buffer is full
        if (pBuf->counter == MAX_BUF) {
            printf("Producer: Buffer full. Waiting.....\n");
            while (pBuf->counter == MAX_BUF) // Busy-wait until space becomes available
                ;
        }

        // Critical section: Produce an item and add it to the buffer
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;       // Generate random data
        pBuf->buf[pBuf->in].data = data;     // Add data to the buffer at the 'in' index
        pBuf->in = (pBuf->in + 1) % MAX_BUF; // Move 'in' index (circular buffer)
        pBuf->counter++;                     // Increment buffer counter

        usleep(data); // Simulate production delay
    }

    // Print summary of produced items
    printf("Producer: Produced %d items.....\n", i);

    sleep(2); // Wait before printing final buffer state

    // Print the final state of the buffer
    printf("Producer: %d items in buffer.....\n", pBuf->counter);

    // Remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl"); // Handle error if shared memory deletion fails
        exit(1);
    }
}
