#include <stdio.h>

main()
{
	char c, s[80];
	int i; long l;
	float f; double d;
	
	scanf("%c %s %d %ld %f %lf", &c, s, &i, &l, &f, &d); 
	printf("Output: %c %s %d %ld %.4f %.2lf\n", c, s, i, l, f, d); 

	c = getchar(); // read a single character from the input
	putchar(c); // print the character to the output

	gets(s); // read a string from the input
    // gets() is considered unsafe and has been removed from the C!! standard due to potential buffer overflow issues
    // it's better to use fgets() instead
    // fgets(s, sizeof(s), stdin);
    // this reads at most sizeof(s) - 1 characters from the input and stores them in the string s
	puts(s); // print the string to the output
}
