#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "synclib.h"

#define	NLOOPS	10
#define	SIZE	sizeof(long)


int
update(long *ptr) // Increment the value of the pointer
{
	return ((*ptr)++);
}

main()
{
	int		fd, i, counter;
	pid_t	pid;
	caddr_t	area; // Pointer to the shared memory area

	if ((fd = open("/dev/zero", O_RDWR)) < 0)  { // Open /dev/zero for read and write
		perror("open");
		exit(1);
	}
	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) // Map the shared memory area of size SIZE with read and write permissions, shared between processes
		== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}
	close(fd);

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
	}
	else  { // Child process
		for (i = 1 ; i < NLOOPS ; i += 2)  {
			WAIT_PARENT(); // Wait for the parent process to signal
			if ((counter = update((long *) area)) != i)  { // Update the counter and check if it is equal to i
				fprintf(stderr, "Counter mismatch\n"); // Writes the error message to stderr (file stream)
				exit(1);
			}
			printf("Child : counter=%d\n", counter); // Print the counter
			TELL_PARENT(); // Tell the parent process to continue
		}
	}
}