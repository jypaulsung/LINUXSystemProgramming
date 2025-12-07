#include <stdio.h>
#include <sys/types.h> // data type definitions
#include <unistd.h> // fork()
#include <sys/wait.h> // wait()

main()
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		printf("I'm a child\n");
		sleep(2);
	}
	else  { // parent
		wait(&status); // wait for the child process to terminate and store the exit status in "status"
		printf("A child killed with %d status\n", status); // print the status of the child process
	}
}