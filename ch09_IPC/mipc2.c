#include <stdio.h> 
#include <sys/types.h> 
#include <sys/shm.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include "synclib.h"

#define	NLOOPS	10
#define	SIZE	sizeof(long)


int
update(long *ptr) // Increment the value pointed to by ptr and return the original value
{
	return ((*ptr)++);
    // (*ptr) : Dereference the pointer to access the value it points to
    // ++ : post-increment operator (increment the value by 1, but return the original value before the increment)
}

main()
{
	int		shmid, i, counter;
    pid_t	pid;
    long    *area; // Pointer to the shared memory segment

	// Create shared memory segment
    if ((shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666)) < 0)  {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory segment
    if ((area = (long *)shmat(shmid, NULL, 0)) == (long *) -1)  {
        perror("shmat");
        exit(1);
    }

	TELL_WAIT(); // Syncronize the parent and child processes
	if ((pid = fork()) < 0)  { // Create child process
		perror("fork");
		exit(1);
	}
	else if (pid > 0)  { // Parent process
		for (i = 0 ; i < NLOOPS ; i += 2)  { // Increment the counter by 2
			if ((counter = update((long *) area)) != i)  { // Update the counter and check if it is equal to i
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			} 
			printf("Parent: counter=%d\n", counter); // Print the counter
			TELL_CHILD(); // Tell the child process to continue
			WAIT_CHILD(); // Wait for the child process to print the counter
		}

        // Detach and remove shared memory segment
        if (shmdt(area) < 0) {
            perror("shmdt");
            exit(1);
        }
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
            exit(1);
        }
	}
	else  {
		for (i = 1 ; i < NLOOPS ; i += 2)  {
			WAIT_PARENT(); // Wait for the parent process to signal
			if ((counter = update((long *) area)) != i)  { // Update the counter and check if it is equal to i
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Child : counter=%d\n", counter); // Print the counter
			TELL_PARENT(); // Tell the parent process to continue
		}

        // Detach shared memory segment
        if (shmdt(area) < 0) {
            perror("shmdt");
            exit(1);
        }
	}
}