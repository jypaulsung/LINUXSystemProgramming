#include <stdio.h>
#include <stdlib.h> // system()

main()
{
	int		status;

	if ((status = system("date")) < 0)  { // execute the date command using system() and store the exit status in "status"
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status); // print the exit status

	if ((status = system("nosuchcommand")) < 0)  { // attempt to execute a non-existent command using system()
		// this results in an error message and an exit status of 127
        // exit status of 127 indicates that the command was not found
        perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	if ((status = system("who; exit 44")) < 0)  { // execute who command and exit with status 44 using system()
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status); // print the exit status of the "who" command and the subsequent exit status 44
}