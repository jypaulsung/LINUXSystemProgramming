#include <stdio.h>

#define	MAX_BUF	256 // define the buffer size


// Read a text file and print its contents with line numbers
main(int argc, char *argv[])
{
	FILE	*fp; // file pointer
	char	buf[MAX_BUF]; // buffer with MAX_BUF size
	int		line; // line number

	if (argc != 2)  { // check if the number of arguments is 2 (program name, filename)
		printf("Usage: %s filename\n", argv[0]); 
		exit(1);
	}

	if ((fp = fopen(argv[1], "rt")) == NULL)  { // open the file in read text mode
		perror("fopen");
		exit(1);
	}

	line = 1; // initialize the line number to 1
	while (fgets(buf, MAX_BUF, fp))  { // read lines from the file to the buffer
		printf("%4d: %s", line++, buf); // print the line number and the line
	}

	fclose(fp);
}