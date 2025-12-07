#include <stdio.h>

main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s old new\n", argv[0]);
		exit(1);
	}

	if (rename(argv[1], argv[2]) < 0)  { // rename the file specified by argv[1] to the name specified by argv[2]
		perror("rename");
		exit(1);
	}
}