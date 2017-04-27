#ifndef _CLASSA_H_
#define _CLASSA_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// ----------------------------------------------

typedef struct ClassA ClassA;

ClassA* ClassA_create(void);
void ClassA_doWork(ClassA* self, int money);
int ClassA_getProgress(ClassA* self);
void ClassA_destroy(ClassA* self);

// ----------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _CLASSA_H_
