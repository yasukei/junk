#ifndef _CLASSB_H_
#define _CLASSB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// ----------------------------------------------

#include "classA.h"

typedef struct ClassB ClassB;

ClassB* ClassB_create(ClassA* classA);
void ClassB_destroy(ClassB* self);

// ----------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _CLASSB_H_
