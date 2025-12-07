#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
	int		pid;

	if ((pid = fork()) < 0)  { // create a child process
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  { // if the return value of fork() is 0, it is the child process
		/* child */
		printf("I am %d. My parent is %d.\n", getpid(), getppid());
	}
	else  { // if the return value of fork() is not 0, it is the parent process
		/* parent */
		printf("I am %d. My child is %d.\n", getpid(), pid);
	}
}