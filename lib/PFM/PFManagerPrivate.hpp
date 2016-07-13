#include <stdio.h>
#include <string>

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

using namespace std;

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------
//void __printErrnoDetails(int errnoSv);

// ------------------------------------------------------------------
// class
// ------------------------------------------------------------------
class PFMProfile
{
	public:
		static bool getValueString(
				const string& filepath,
				const string& section,
				const string& entry,
				string& valueString
				);
		static bool getValueBool(
				const string& filepath,
				const string& section,
				const string& entry,
				bool& valueBool
				);
};

