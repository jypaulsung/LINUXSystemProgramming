#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	umask(0); // set fild mode creation mask to 0 (no permission masked out)
    // file will be created with the exact permission specified in the creat()
	if (creat("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | // create file with read/write permission for user, group, and others
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}

	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // set file mode creation mask to restrict read and write for the group and others
	if (creat("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | // due to the umask on the line above, the actual permissions will be rw-r-----
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}
}