#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int		Var = 6;
char	Buf[] = "a write to stdout\n";

main()
{
	int		var;
	pid_t	pid; // process ID

	var = 88;
	write(STDOUT_FILENO, Buf, sizeof(Buf)-1); // write the contents of the Buf array to the standard output (STDOUT_FILENO)
    // sizeof(Buf) returns the size of the array in bytes, -1 to exclude the null terminator
	printf("Before fork\n");

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  { // executed by the child process
		/* child */
		Var++;
		var++;
	}
	else  { // executed by the parent process
		/* parent */
		sleep(2);
	}

	printf("pid = %d, Var = %d, var = %d\n", getpid(), Var, var); // print the process ID, Var, and var
}