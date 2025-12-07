#include <stdio.h>
#include <fcntl.h> // file control options for open
#include <unistd.h> // POSIX OS API

#define	MAX_BUF	1024 // buffer size

main(int argc, char *argv[]) // copies a file
{
	int 	fd1, fd2, count; // file descriptors and number of bytes read
	char	buf[MAX_BUF]; // buffer

	if (argc != 3)  { // check if the number of arguments is 3 (program name, source, destination)
		printf("Usage: %s source destination\n", argv[0]); // print usage
		exit(1);
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0)  { // open the source file in read-only mode
		perror("open");
		exit(1);
	}

	if ((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)  { // open the destination file in write-only mode, create if it doesn't exist, truncate if it does
    // 0644 : read/write for owner, read-only for group and others
		perror("open");
		exit(1);
	}

	while ((count = read(fd1, buf, MAX_BUF)) > 0)  { // read from the source file to the buffer
		write(fd2, buf, count); // copy all the bytes from the buffer to the destination file
	}

    // Close the files
	close(fd1);
	close(fd2);
}