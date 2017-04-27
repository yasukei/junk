#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "classA.h"

#include "classB.h"

// ----------------------------------------------
// ClassB
// ----------------------------------------------
struct ClassB
{
	ClassA* classA;
};

// ----------------------------------------------
ClassB* ClassB_create(ClassA* classA)
{
	ClassB* self;

	self = UtilMalloc(sizeof(ClassB));
	if(self == NULL)
	{
		return NULL;
	}

	self->classA = classA;
	return self;
}

// ----------------------------------------------
void ClassB_destroy(ClassB* self)
{
	UtilFree(self);
}

