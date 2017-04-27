#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#include "classA.h"

// ----------------------------------------------
// ClassA
// ----------------------------------------------
struct ClassA
{
	int progress;
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
	self->progress = 0;
	return self;
}

// ----------------------------------------------
void ClassA_doWork(ClassA* self, int money)
{
	if(money < 0)
	{
		return;
	}

	self->progress += money / 2;
}

// ----------------------------------------------
int ClassA_getProgress(ClassA* self)
{
	return self->progress;
}

// ----------------------------------------------
void ClassA_destroy(ClassA* self)
{
	UtilFree(self);
}

