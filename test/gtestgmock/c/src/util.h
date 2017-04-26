#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
// ----------------------------------------------

void* UtilMalloc(size_t size);
void UtilFree(void* ptr);

// ----------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _UTIL_H_

