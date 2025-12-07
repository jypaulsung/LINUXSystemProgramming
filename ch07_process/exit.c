#include <stdio.h>
#include <stdlib.h> // exit(), atexit()

void
myexit1()
{
	printf("first exit handler\n");
}

void
myexit2()
{
	printf("second exit handler\n");
}

// atexit() registers a function to be called at exit
// the functions are called in the reverse order of registration
main()
{
	if (atexit(myexit2) != 0)  { // register myexit2 to be called at exit
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  { // register myexit1 to be called at exit (first time)
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  { // register myexit1 to be called at exit (second time)
		perror("atexit");
		exit(1);
	}

	printf("main is done\n");
}