#include <stdio.h>

#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");
}

main()
{
	char	cmd[MAX_CMD];
	int		pid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);
		if (cmd[0] == 'q')
			break;

		if ((pid = fork()) < 0)  {
			perror("fork");
			exit(1);
		}
		else if (pid == 0)  {
            printf("This is the child process.\n");
			DoCmd(cmd);
			exit(0);
		}
// does not compile this part because of 0
// it will be compiled if the 0 is changed to 1
#if 0 
		else  {
			wait(NULL);
            printf("Now it's the parent process.\n");
		}
#endif
	}
}