#include <stdio.h>

int
strlen_p(char *str) // take a pointer to a character as an argument and return the length of the string
{
	int	len = 0;

	while (*str++)  { // increment the pointer until the null terminator is reached
		len++;
	}
	return len;
}

int
strlen_a(char str[]) // take an array of characters as an argument and return the length of the string
{
	int i;

	for (i = 0 ; str[i] != '\0' ; i++) // increment the index until the null terminator is reached
		;
	return i;
}

void
strcpy_p(char *dst, char *src) // take pointers to the source and destination strings as arguments and copy the source to the destination
{
	while (*src)  { // copy characters from the source to the destination until the null terminator is reached
		*dst++ = *src++;
	}
	*dst = *src; // copy the null terminator to the destination string
}

void
strcpy_a(char dst[], char src[]) // take arrays of characters as arguments and copy the source to the destination
{
	int i;

	for (i = 0 ; src[i] != '\0' ; i++) // copy characters from the source to the destination until the null terminator is reached
		dst[i] = src[i];
	dst[i] = src[i]; // copy the null terminator to the destination string
}

void
strcat_p(char *dst, char *src) // take pointers to the source and destination strings as arguments and append the source to the destination
{
	while (*dst++) // move the destination pointer to the end of the string
		;
	dst--; // move the pointer back to the null terminator
	while (*src)  { // append characters from the source to the destination until the null terminator is reached
		*dst++ = *src++;
	}
	*dst = *src; // append the null terminator to the destination string
}

void
strcat_a(char dst[], char src[]) // take arrays of characters as arguments and append the source to the destination
{
	int i, j;

	for (i = 0 ; dst[i] != '\0' ; i++) // move the destination index to the end of the string
		;
	for (j = 0 ; src[j] != '\0' ; j++) // append characters from the source to the destination until the null terminator is reached
		dst[i+j] = src[j];
	dst[i+j] = src[j]; // append the null terminator to the destination string
}

main()
{
	int		len1, len2;
	char	str1[20], str2[20];

	len1 = strlen_p("Hello");
	len2 = strlen_a("Hello");
	printf("strlen: p=%d, a=%d\n", len1, len2);

	strcpy_p(str1, "Hello");
	strcpy_a(str2, "Hello");
	printf("strcpy: p=%s, a=%s\n", str1, str2);

	strcat_p(str1, ", World!");
	strcat_a(str2, ", World!");
	printf("strcat: p=%s, a=%s\n", str1, str2);
}