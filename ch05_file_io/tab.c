#include <stdio.h>

enum { FALSE, TRUE };

// Convert tabs to 4 spaces
// Read characters from the source file and write them to the destination file with .t extension
void
conv(char *fname)
{
	FILE	*src, *dst;
	char	fdest[40];
	int		ch, first;

	if ((src = fopen(fname, "rt")) == NULL)  { // open the source file in read text mode
		perror("fopen");
		return;
	}

	strcpy(fdest, fname); // copy the filename to the destination filename
	strcat(fdest, ".t"); // append ".t" to the destination filename
	if ((dst = fopen(fdest, "wt")) == NULL)  { // open the destination file in write text mode
		perror("fopen");
		return;
	}

	first = TRUE;
	while ((ch = fgetc(src)) != EOF)  { // read characters from the source file until EOF
		if (first && ch == '\t')  { // if it's the first character and it's a tab character 
			fputc(' ', dst);
			fputc(' ', dst);
			fputc(' ', dst);
			fputc(' ', dst);
		}
		else  { // if it's not the first character or it's not a tab character
			fputc(ch, dst); // write the character to the destination file
			if (first)
				first = FALSE;
			if (ch == '\n') // if the current character is a newline character, set first to TRUE
				first = TRUE;
		}
	}

	fclose(src);
	fclose(dst);
}

main(int argc, char *argv[])
{
	while (--argc)  { // iterates through the arguments in reverse order
		conv(argv[argc]); // convert the file
	}
}