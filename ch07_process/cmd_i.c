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
	char	cmd[MAX_CMD]; // command buffer

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	 // read a command from the standard input
		if (cmd[0] == 'q') // exit the loop if the command is 'q'
			break;

		DoCmd(cmd); // execute the command
	}
}