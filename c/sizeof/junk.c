#include <stdio.h>

int main(void)
{
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;

	printf("sizeof(char):   %lu\n", sizeof(char));
	printf("sizeof(c):      %lu\n", sizeof(c));
	printf("sizeof(short):  %lu\n", sizeof(short));
	printf("sizeof(s):      %lu\n", sizeof(s));
	printf("sizeof(int):    %lu\n", sizeof(int));
	printf("sizeof(i):      %lu\n", sizeof(i));
	printf("sizeof(long):   %lu\n", sizeof(long));
	printf("sizeof(l):      %lu\n", sizeof(l));
	printf("sizeof(float):  %lu\n", sizeof(float));
	printf("sizeof(f):      %lu\n", sizeof(f));
	printf("sizeof(double): %lu\n", sizeof(double));
	printf("sizeof(d):      %lu\n", sizeof(d));
	printf("\n");
	printf("sizeof('A'):    %lu\n", sizeof('A'));
	printf("sizeof(-1):     %lu\n", sizeof(-1));
	printf("sizeof(0):      %lu\n", sizeof(0));
	printf("sizeof(1):      %lu\n", sizeof(1));
	printf("sizeof(1.0):    %lu\n", sizeof(1.0));
	return 0;
}
