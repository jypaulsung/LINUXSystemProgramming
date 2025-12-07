#include <stdio.h>

main(int argc, char *argv[], char *envp[]) // argc: number of command-line arguments, argv: array of command-line arguments, envp: array of environment variables
{
	int			i;
	char		**p; // pointer to a pointer to a character (used to iterate over the environment variables)
	extern char	**environ; // pointer to an array of strings representing the environment variables
    // environ is a global variable that contains the environment variables
    // it is declared as extern to access the environment variables directly

	printf("List command-line arguments\n"); // print the command-line arguments
	for (i = 0 ; i < argc ; i++)  { // iterate over each command-line argument
		printf("%s\n", argv[i]);
	}

	printf("\n");
	printf("List environment variables from environ variable\n"); // print the environment variables using the environ variable
	for (p = environ ; *p != NULL ; p++)  { // iterate over each environment variable
		printf("%s\n", *p);
	}

	printf("\n");
	printf("List environment variables from envp variable\n"); // print the environment variables using the envp variable
	for (p = envp ; *p != NULL ; p++)  {
		printf("%s\n", *p);
	}
}