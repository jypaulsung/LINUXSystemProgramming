#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void
CharAtaTime(char *str) // print the characters in str one at a time
{
	char	*ptr;
	int		c, i;

	setbuf(stdout, NULL); // disable buffering for the standard output, ensuring that characters are printed immediately
	for (ptr = str ; c = *ptr++ ; )  { // iterate over each character in str
		for(i = 0 ; i < 999999 ; i++) // introduce a delay to simulate slow output
			;
		putc(c, stdout); // print each character
	}
}

main()
{
	pid_t	pid;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		CharAtaTime("output from child\n");
	} 
	else  {
		CharAtaTime("output from parent\n");
	}
}