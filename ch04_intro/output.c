#include <stdio.h> // standard input/output library

main()
{
	char c = 'a', s[] = "hello";
	int i = 100; long l = 99999;
	float f = 3.14; double d = 99.999;
	int *p = &i;
	
	printf("Output: %c %s %d %#X %ld %.4f %.2lf %p\n", c, s, i, i, l, f, d, p); // %#X : hexadecimal format with a 0X prefix, %.4f : floating-point number with 4 decimal places
    // %.2lf : double-precision floating-point number with 2 decimal places
	putchar(c); // print a single character
	puts(s); // print a string followed by a newline character
}
