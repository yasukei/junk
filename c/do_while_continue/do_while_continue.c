#include <stdio.h>

#define TRUE	1
#define FALSE	0
typedef unsigned char BOOL;

int main(void)
{
	BOOL inLoop = TRUE;

	printf("--- loop begin ---\n");
	do
	{
		if(inLoop)
		{
			printf("--- in loop ---\n");
			continue;
		}
	}
	while(FALSE);
	printf("--- loop end ---\n");

	return 0;
}

