#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

// Make a file with a "hole" in it
// the file will be 1024 bytes long, with only a null character at the end
// the rest of the file will be empty (implicitly zero-filled: nothing is written to the file, not even a null character)
main()
{
	int 	fd; // to hold the file descriptor
	char	ch = '\0'; // null character

	if ((fd = creat("file.hole", 0400)) < 0)  { // create a file with read-only permission
		perror("creat");
		exit(1);
	}

	if (lseek(fd, 1023, SEEK_SET) < 0)  { // move the file pointer to 1023 bytes away from the beginning
        // SEEK_SET: set the file offset to offset bytes
        // will be 1024 bytes long (0-1023)
		perror("lseek");
		exit(1);
	}
	write(fd, &ch, 1); // write a null character to the file at the current file offset (1023rd byte)

	close(fd);
}