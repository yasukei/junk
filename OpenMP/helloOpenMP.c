#include <stdio.h>
#include <omp.h>

int main(void)
{
	printf("thread num: [%d]\n", omp_get_num_threads());
	
	#pragma omp parallel
	{
		#pragma omp single
		printf("thread num: [%d]\n", omp_get_num_threads());
		
		printf("thread%d: hello openMP\n", omp_get_thread_num());
	}
	
	return 0;
}
