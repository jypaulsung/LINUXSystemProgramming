#include <stdio.h>
#include <unistd.h>

#define	MAX_BUF	256

main(int argc, char *argv[])
{
	char	buf[MAX_BUF];

	if (getcwd(buf, MAX_BUF) == NULL)  { // get the current working directory and store it in buf
		perror("getcwd");
		exit(1);
	}

	printf("%s\n", buf); // print the current working directory
}