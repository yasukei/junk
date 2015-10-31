#include <stdio.h>
#include "library1.h"

void Library1_sayHello(void)
{
	printf("hello, %s@%s\n", __func__, __FILE__);
}
