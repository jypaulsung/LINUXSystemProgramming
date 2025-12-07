#include <stdio.h>
#include <unistd.h>

main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (chdir(argv[1]) < 0)  { // change the current working directory to the directory specified by argv[1]
		perror("chdir");
		exit(1);
	}
}