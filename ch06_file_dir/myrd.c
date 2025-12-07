#include <stdio.h>
#include <unistd.h>

main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (rmdir(argv[1]) < 0)  { // remove the directory specified by argv[1]
		perror("rmdir");
		exit(1);
	}
}