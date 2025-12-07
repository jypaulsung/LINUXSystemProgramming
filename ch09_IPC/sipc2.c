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
	pData = ptr + sizeof(int); // pData is set to point to the data portion of the shared memory segment
	sprintf(pData, "This is a request from %d.", getpid()); // write a request containing the process ID to pData
	*pInt = 1; // set to 1 to indicate that a request has been made
	printf("Sent a request.....");

	while ((*pInt) == 1) // busy-wait for *pInt to become 0 (i.e., a reply has been received)
		;

	printf("Received reply: %s\n", pData); // print the reply stored in pData
}