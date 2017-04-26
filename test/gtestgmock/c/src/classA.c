#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#include "classA.h"

// ----------------------------------------------
// ClassA
// ----------------------------------------------
struct ClassA
{
	int dummy;
};

// ----------------------------------------------
ClassA* ClassA_create(void)
{
	ClassA* self;

	self = UtilMalloc(sizeof(ClassA));
	if(self == NULL)
	{
		return NULL;
	}
	return self;
}

// ----------------------------------------------
void ClassA_destroy(ClassA* self)
{
	UtilFree(self);
}

