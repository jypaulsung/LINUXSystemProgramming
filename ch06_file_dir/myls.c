#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

// print the names of all files and directories in the current directory
main()
{
	DIR				*dp; // directory stream
	struct dirent	*dep; // directory entry

	if ((dp = opendir(".")) == NULL)  { // open the current directory
		perror("opendir");
		exit(0);
	}

	while (dep = readdir(dp))  { // read each directory entry
		printf("%s\n", dep->d_name); // print the name of the directory entry
	}
	
	closedir(dp); // close the directory stream
}