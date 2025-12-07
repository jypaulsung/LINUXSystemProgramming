#include <stdio.h>
#include <pthread.h> // pthread_create(), pthread_exit()

#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");

	pthread_exit(NULL); // terminate the calling thread
}

main()
{
	char		cmd[MAX_CMD];
	pthread_t	tid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	// read a command from the standard input
		if (cmd[0] == 'q')
			break;

		if (pthread_create(&tid, NULL, (void *)DoCmd, (void *)cmd) < 0)  { // create a new thread with the identifier tid
            // the new thread will execute the DoCmd() function with cmd as its argument
            // NULL is passed as the second argument to pthread_create() to use the default thread attributes
			perror("pthread_create");
			exit(1);
		}

#if 0 // if uncommented, the parent process will wait for the child thread to terminate
// will wait for each child thread to complete before continuing to the next iteration of the loop
// this means that the parent process will not prompt for a new command until the current child thread has finished executing
		pthread_join(tid, NULL);
#endif
	}
}