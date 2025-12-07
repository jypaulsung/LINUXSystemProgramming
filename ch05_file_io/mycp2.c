#include <stdio.h>

#define	MAX_BUF	1024

main(int argc, char *argv[])
{
	FILE	*src, *dst;
	char	buf[MAX_BUF];
	int		count;

	if (argc != 3)  { // check if the number of arguments is 3 (program name, source, destination)
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rb")) == NULL)  { // open the source file in read binary mode
		perror("fopen");
		exit(1);
	}

	if ((dst = fopen(argv[2], "wb")) == NULL)  { // open the destination file in write binary mode
		perror("fopen");
		exit(1);
	}

	while ((count = fread(buf, 1, MAX_BUF, src)) > 0)  { // (buffer, size of each element, number of elements, input stream)
		fwrite(buf, 1, count, dst);
	}

	fclose(src);
	fclose(dst);
}