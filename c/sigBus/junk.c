#include <stdio.h>

void __printCharArray(
	char array[],
	int length
	)
{
	int i;

	for(i = 0; i < length; i++)
	{
		printf("array[%d] : %c\n", i, array[i]);
	}
}

int main(void)
{
	char c[4] = {0};
	int* ptr = (int*)&c[1];

	__printCharArray(c, sizeof(c) / sizeof(c[0]));
	*ptr = 65535;
	__printCharArray(c, sizeof(c) / sizeof(c[0]));

	printf("SIGBUS didn't occur\n");
	return 0;
}
