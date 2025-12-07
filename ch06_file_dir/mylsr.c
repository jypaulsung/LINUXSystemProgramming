#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define	MAX_PATH    256

void
JustDoIt(char *path) // take a directory path as an argument
{
	DIR				*dp; // directory stream
	struct dirent	*dep; // directory entry
	struct stat		statbuf; // file status information
	char			fullpath[MAX_PATH]; // full path of files and directories

	if ((dp = opendir(path)) == NULL)  { // open the directory specified by path
		perror("opendir");
		exit(0);
	}

	printf("\n%s:\n", path); // print the directory name
	while (dep = readdir(dp))  { // read each directory entry, loop continues until readdir() returns NULL
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0) // skip the current and parent directory
			continue;
		printf("%s\n", dep->d_name); // print the name of the directory entry
	}

	rewinddir(dp); // reset the position of the directory stream to the beginning
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue;
		strcpy(fullpath, path); // copy the directory path to fullpath
		strcat(fullpath, "/"); // append a slash to the directory path
		strcat(fullpath, dep->d_name); // append the name of the directory entry to the directory path
		if (lstat(fullpath, &statbuf) < 0)  { // get status of the directory entry specified by fullpath
			perror("lstat");
			exit(1);
		}
		if (S_ISDIR(statbuf.st_mode))  { // check if the directory entry is a directory
			JustDoIt(fullpath); // recursively calls the JustDoIt() function to process the subdirectory
		}
	}
	
	closedir(dp);
}

main()
{
	JustDoIt("."); // start the recursive process from the current directory
}