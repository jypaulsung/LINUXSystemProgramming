// Busy waiting version

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"


main()
{
	int		shmid;
	char	*ptr, *pData;
	int		*pInt;

	// Allocate shared memory segment with key, size, and mode
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { 
		perror("shmget");
		exit(1);
	}
	// Attach shared memory segment to the process's address space
	if ((ptr = shmat(shmid, 0, 0)) == (void *) -1)  { 
		perror("shmat");
		exit(1);
	}

	pInt = (int *)ptr; // pInt points to the first byte of the shared memory segment
	while ((*pInt) == 0) // busy-wait for *pInt to become non-zero (i.e., a request has been made)
		;

	pData = ptr + sizeof(int); // pData is set to point to the data portion of the shared memory segment
	printf("Received request: %s.....", pData); // print the request stored in pData
	sprintf(pData, "This is a reply from %d.", getpid()); // write a reply containing the process ID to pData
	*pInt = 0; // set to 0 to indicate that the reply has been sent
	printf("Replied.\n");

	sleep(1); // wait for the client to read the reply

	// Mark the shared memory segment for deletion
	if (shmctl(shmid, IPC_RMID, 0) < 0)  { 
		perror("shmctl");
		exit(1);
	}
}