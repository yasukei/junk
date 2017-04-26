#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#include "classA.h"

// ----------------------------------------------
// InnerClassB
// ----------------------------------------------
typedef struct InnerClassB InnerClassB;
struct InnerClassB
{
	int dummy;
};

// ----------------------------------------------
static InnerClassB* InnerClassB_create(void)
{
	InnerClassB* self;

	self = UtilMalloc(sizeof(InnerClassB));
	if(self == NULL)
	{
		return NULL;
	}
	return self;
}

// ----------------------------------------------
static void InnerClassB_destroy(InnerClassB* self)
{
	UtilFree(self);
}

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

