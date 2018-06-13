#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	void* ptr1;
	void* ptr2;
	void* ptr3;

	fprintf(stdout, "hello main\n");
	fflush(stdout);

	ptr1 = malloc(4);
	ptr2 = calloc(1, 8);
	ptr3 = realloc(NULL, 12);

	free(ptr1);
	free(ptr2);
	free(ptr3);

	return 0;
}

