#include <stdio.h>

// ------------------------------------------------------------------
// define
// ------------------------------------------------------------------
#define DEBUG
#define CHECKPOINT printf("CP: %s@%d\n", __FILE__, __LINE__);
#define CHECK_POINT(fmt, ...) printf("CP: %s@%d, " fmt, __FILE__, __LINE__, ##__VA_ARGS__);

#ifdef DEBUG
#define DEBUGPOINT CHECKPOINT;
#define DEBUG_POINT(fmt, ...) CHECK_POINT(fmt, ##__VA_ARGS__);
#else
#define DEBUGPOINT
#define DEBUG_POINT(fmt, ...)
#endif /* DEBUG */

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------
void __printErrnoDetails(int errnoSv);

