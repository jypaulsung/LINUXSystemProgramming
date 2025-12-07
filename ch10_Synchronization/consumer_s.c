#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include "prodcons.h"

main()
{
	BoundedBufferType	*pBuf;
	int					shmid, i, data;
	int					emptySemid, fullSemid, mutexSemid;

	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // Get shared memory
		perror("shmget");
		exit(1);
	}
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  { // Attach shared memory
		perror("shmat");
		exit(1);
	}

	if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0)  { // Get semaphore id for empty
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((fullSemid = semInit(FULL_SEM_KEY)) < 0)  { // Get semaphore id for full
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0)  { // Get semaphore id for mutex
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}

	if (semInitValue(emptySemid, MAX_BUF) < 0)  { // Initialize semaphore value for empty
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}
	if (semInitValue(fullSemid, 0) < 0)  { // Initialize semaphore value for full
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}
	if (semInitValue(mutexSemid, 1) < 0)  { // Initialize semaphore value for mutex
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}

	srand(0x9999);
	for (i = 0 ; i < NLOOPS ; i++)  {
		if (semWait(fullSemid) < 0)  { // Wait for semaphore full
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		if (semWait(mutexSemid) < 0)  { // Wait for semaphore mutex
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		printf("Consumer: Consuming an item.....\n");
		data = pBuf->buf[pBuf->out].data; // Retrieve the data from the buffer at the position indicated by pBuf->out
		// pBuf->buf : access the buf element within the BoundedBufferType structure
		// pBuf->buf[pBuf->out].data : access the data field of the buffer element at the out index
		pBuf->out = (pBuf->out + 1) % MAX_BUF; // Update the out index to the next buffer element
		// modulo operation ensures that the out index wraps around to the beginning of the buffer when it reaches the end (circular buffer)
		pBuf->counter--; // Keep track of the number of items in the buffer

		if (semPost(mutexSemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		if (semPost(emptySemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}

		usleep((rand()%100)*10000);
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}