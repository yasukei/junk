#include <stdio.h>
#include <omp.h>

int main(void)
{
	int i;
	
	#pragma omp parallel for
	for(i = 0; i < 10; i++)
	{
		printf("hello%d\n", i);
	}
	
	return 0;
}
