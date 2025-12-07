#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h" // Include the header file for shared memory keys and structures

main()
{
    BoundedBufferType    *pBuf; // Pointer to the shared memory buffer structure
    int                    shmid, i, data;

    // Allocate shared memory with the specified key and size
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
        perror("shmget"); // Handle error if shared memory creation fails
        exit(1);
    }

    // Attach the shared memory to the process's address space
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  {
        perror("shmat"); // Handle error if attaching shared memory fails
        exit(1);
    }

    srand(0x9999); // Seed the random number generator

    // Loop to consume items from the shared buffer
    for (i = 0 ; i < NLOOPS ; i++)  {
        // Check if the buffer is empty and wait if it is
        if (pBuf->counter == 0)  {
            printf("Consumer: Buffer empty. Waiting.....\n");
            while (pBuf->counter == 0) // Busy-wait until the buffer has data
                ;
        }

        printf("Consumer: Consuming an item.....\n");
        // Consume data from the buffer at the current 'out' index
        data = pBuf->buf[pBuf->out].data;
        // Move the 'out' index to the next position (circular buffer)
        pBuf->out = (pBuf->out + 1) % MAX_BUF;
        // Decrement the counter to reflect one less item in the buffer
        pBuf->counter--;

        // Simulate consumption delay
        usleep((rand()%100)*10000); // Sleep for a random period
    }

    // Print a summary after consuming items
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}
