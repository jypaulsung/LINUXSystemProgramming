#include <stdio.h>
#include <unistd.h>

#define	MAX_BUF	1024

long
filesize(FILE *fp) // takes a file pointer as an argument and returns the size of the file
{
	long	cur, size;

	cur = ftell(fp); // save the current file position
	fseek(fp, 0L, SEEK_END); // move the file pointer to the end of the file
	size = ftell(fp); // get the current file position, which is the size of the file
	fseek(fp, cur, SEEK_SET); // restore the file pointer to the original position
	return(size);
}

main(int argc, char *argv[])
{
	FILE	*src, *dst1, *dst2;
	char	buf[MAX_BUF];
	int		count, size;

	if (argc != 4)  { // check if the number of arguments is 4 (program name, source, destination1, destination2)
		fprintf(stderr, "Usage: %s source dest1 dest2\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst1 = fopen(argv[2], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst2 = fopen(argv[3], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	size = filesize(src) / 2; // get the size of the source file and divide it by 2

	while (size > 0)  { // read the first half of the file and write it to the first destination file
		count = (size > MAX_BUF) ?  MAX_BUF : size; // read the smaller of the size or MAX_BUF
		fread(buf, 1, count, src); // read "count" bytes from the source file into the buffer
		fwrite(buf, 1, count, dst1); // write "count" bytes from the buffer to the destination file
		size -= count; // decrement the size by the number of bytes read, loop continues until size is 0
	}

	while ((count = fread(buf, 1, MAX_BUF, src)) > 0)  { // read the second half of the file and write it to the second destination file
		fwrite(buf, 1, count, dst2);
	}

	fclose(src);
	fclose(dst1);
	fclose(dst2);
}