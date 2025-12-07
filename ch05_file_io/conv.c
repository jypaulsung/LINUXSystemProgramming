#include <stdio.h>
#include <string.h>
#include "record.h"

// Read records from a text file and write them to a binary file
// Read each line, remove the newline character, and write the record to the destination file in a binary format
main(int argc, char *argv[])
{
	FILE	*src, *dst;
	Record	rec;

	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source dest\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rt")) == NULL)  { // open the source file in read text mode
		perror("fopen");
		exit(1);
	}
	if ((dst = fopen(argv[2], "wb")) == NULL)  { // open the destination file in write binary mode
		perror("fopen");
		exit(1);
	}

	while (fgets(rec.stud, SMAX, src))  { // read SMAX - 1 (because 1 null byte is added at the end) characters from the source file into the student field of the record
		*strchr(rec.stud, '\n') = '\0'; // replace the newline character with a null terminator
		fgets(rec.num, NMAX, src);
		*strchr(rec.num, '\n') = '\0';
		fgets(rec.dept, DMAX, src);
		*strchr(rec.dept, '\n') = '\0';

		fwrite(&rec, sizeof(rec), 1, dst);
	}

	fclose(src);
	fclose(dst);
}