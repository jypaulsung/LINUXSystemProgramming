#include <stdio.h>

#define	MAX_CMD		256

void
DoCmd(char *cmd) // simulate the execution of a command
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");
}

main()
{
	char	cmd[MAX_CMD];
	int		pid; // to store the process ID

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	// read a command from the standard input and store it in cmd
		if (cmd[0] == 'q') // exit the loop if the first character of the command is 'q'
			break; // break the loop and exit the program

		if ((pid = fork()) < 0)  {
			perror("fork");
			exit(1);
		}
		else if (pid == 0)  { // child process
			DoCmd(cmd);
			exit(0);
		}
#if 0 // this block is currently commented out
// if uncommented, the parent process will wait for the child process to terminate
// will wait for each child process to complete before continuing to the next iteration of the loop
// this means that the parent process will not prompt for a new command until the current child process has finished executing
		else  {
			wait(NULL);
		}
#endif
	}
}
// The parent process will prompt the user to enter a command with printf("CMD> ");
// The command is read from the standard input using fgets() and stored in the cmd buffer
// If the first character of the command is 'q', the loop will break and the program will exit
// If the command does not begin with 'q', a child process is created using fork()
// The child process will execute the DoCmd() function with the command as an argument
// The child process will then exit
// The parent process will wait for the child process to terminate using wait(NULL);
// The parent process will then prompt the user for a new command and repeat the process