#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	struct stat	statbuf;

	if (stat("bar", &statbuf) < 0)  { // get status of the file named "bar" and store the information in statbuf
		perror("stat");
		exit(1);
	}

	if (chmod("bar", (statbuf.st_mode & ~S_IWGRP) | S_ISUID) < 0)  { // change the file permission of "bar" to remove write permission for group and add set-user-ID bit
        // allow the file to be executed with the permission of the file owner instead of the user who runs the file
		// user can perform tasks that require higher privileges (e.g. accessing restricted files, performing administrative tasks, etc.)
        perror("chmod");
		exit(1);
	}

	if (chmod("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)  { // change the file permission of "foo" to read/write for user, read for group and others
		perror("chmod");
		exit(1);
	}
}