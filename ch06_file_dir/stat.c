#include <stdio.h>
#include <sys/types.h> // data type definitions
#include <sys/stat.h> // file status information

main(int argc, char *argv[])
{
	struct stat	statbuf; // stat structure to store file status information
	char		*mode; // file type
	int			i;

	for (i = 1 ; i < argc ; i++)  {
		printf("%s: ", argv[i]);
		if (lstat(argv[i], &statbuf) < 0)  { // get status of the file specified by argv[i]
			perror("lstat"); // if any file cannot be accessed, print error message and continue to the next file
			continue;
		}
		if (S_ISREG(statbuf.st_mode)) // check file type and set mode string accordingly
			mode = "regular";
		else if (S_ISDIR(statbuf.st_mode))
			mode = "directory";
		else if (S_ISCHR(statbuf.st_mode))
			mode = "character special";
		else if (S_ISBLK(statbuf.st_mode))
			mode = "block special";
		else if (S_ISFIFO(statbuf.st_mode))
			mode = "FIFO";
		else if (S_ISLNK(statbuf.st_mode))
			mode = "symbolic link";
		else if (S_ISSOCK(statbuf.st_mode))
			mode = "socket";
		printf("%s\n", mode); // print file type
		printf("\tst_mode = %d\n", statbuf.st_mode); // file mode
		printf("\tst_ino = %d\n", statbuf.st_ino); // inode number
		printf("\tst_dev = %d\n", statbuf.st_dev); // device ID
		printf("\tst_rdev = %d\n", statbuf.st_rdev); // device ID (if special file)
		printf("\tst_nlink = %d\n", statbuf.st_nlink); // number of hard links
		printf("\tst_uid = %d\n", statbuf.st_uid); // user ID of owner
		printf("\tst_gid = %d\n", statbuf.st_gid); // group ID of owner
		printf("\tst_size = %d\n", statbuf.st_size); // file size in bytes
		printf("\tst_atime = %d\n", statbuf.st_atime); // last access time
		printf("\tst_mtime = %d\n", statbuf.st_mtime); // last modification time
		printf("\tst_ctime = %d\n", statbuf.st_ctime); // last status change time
		printf("\tst_blksize = %d\n", statbuf.st_blksize); // block size for file system I/O
		printf("\tst_blocks = %d\n", statbuf.st_blocks); // number of 512B blocks allocated
	}
}