// ------------------------------------------------------------------
// reference:
//  http://en.wikibooks.org/wiki/C_Programming/C_Reference/string.h
// ------------------------------------------------------------------

extern "C"
{
#include <string.h>
}

#include <tuple>
#include "gtest/gtest.h"
using std::vector;
using std::tuple;

// ------------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------------
static bool __isSameName(const char* str1, const char* str2)
{
	return strcmp(str1, str2) == 0 ? true : false;
}

// ------------------------------------------------------------------
// void *memcpy(void *dst, const void *src, size_t n);
// copies n bytes between two memory areas; if there is overlap, the behavior is undefined
// ------------------------------------------------------------------
class memcpy_test : public ::testing::TestWithParam<
			tuple< vector<char>, vector<char>, size_t, vector<char> >
		>
{ 
};

TEST_P(memcpy_test, _)
{
	vector<char>	dst		= std::get<0>(GetParam());
	vector<char>	src		= std::get<1>(GetParam());
	size_t			size	= std::get<2>(GetParam());
	vector<char>	expect	= std::get<3>(GetParam());
	void* ret;

	// target
	ret = memcpy(&dst[1], &src[0], size);

	// check
	EXPECT_EQ(&dst[1], ret);
	EXPECT_EQ(expect, dst);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	memcpy_test,
	testing::Values(
		make_tuple(
			vector<char> {0xF, 0xF, 0xF},	// dst
			vector<char> {0x0},				// src
			size_t(0),						// size
			vector<char> {0xF, 0xF, 0xF}	// expect
		),
		make_tuple(
			vector<char> {0xF, 0xF, 0xF},
			vector<char> {0x0},
			size_t(1),
			vector<char> {0xF, 0x0, 0xF}
		),
		make_tuple(
			vector<char> {0xF, 0xF, 0xF, 0xF},
			vector<char> {0x0, 0x1},
			size_t(2),
			vector<char> {0xF, 0x0, 0x1, 0xF}
		),
		make_tuple(
			vector<char> {0xF, 0xF, 0xF, 0xF, 0xF},
			vector<char> {0x0, 0x1, 0x2},
			size_t(3),
			vector<char> {0xF, 0x0, 0x1, 0x2, 0xF}
		)
	)
);

// ------------------------------------------------------------------
// void *memmove(void *dest, const void *src, size_t n);
// copies n bytes between two memory areas; unlike with memcpy the areas may overlap
// ------------------------------------------------------------------
class memmove_test : public ::testing::TestWithParam<
			tuple< char*, vector<char>, size_t, vector<char> >
		>
{ 
};

TEST_P(memmove_test, dst_is_in_front_of_src)
{
	const ::testing::TestInfo* const test_info =
		  ::testing::UnitTest::GetInstance()->current_test_info();
	vector<char>	dstsrc	= std::get<1>(GetParam());
	size_t			size	= std::get<2>(GetParam());
	vector<char>	expect	= std::get<3>(GetParam());
	void* ret;

	if(! __isSameName(test_info->name(), std::get<0>(GetParam()))) { /* skip */ return; }

	// target
	ret = memmove(&dstsrc[1], &dstsrc[2], size);

	// check
	EXPECT_EQ(&dstsrc[1], ret);
	EXPECT_EQ(expect, dstsrc);
}

TEST_P(memmove_test, dst_is_behind_src)
{
	const ::testing::TestInfo* const test_info =
		  ::testing::UnitTest::GetInstance()->current_test_info();
	vector<char>	dstsrc	= std::get<1>(GetParam());
	size_t			size	= std::get<2>(GetParam());
	vector<char>	expect	= std::get<3>(GetParam());
	void* ret;

	if(! __isSameName(test_info->name(), std::get<0>(GetParam()))) { /* skip */ return; }

	// target
	ret = memmove(&dstsrc[2], &dstsrc[1], size);

	// check
	EXPECT_EQ(&dstsrc[2], ret);
	EXPECT_EQ(expect, dstsrc);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	memmove_test,
	testing::Values(
		make_tuple(
			(char*)"dst_is_in_front_of_src",
							// dst, src
			vector<char> {0xF, 0xF, 0x0, 0xF},	// dstsrc
			size_t(0),							// size
			vector<char> {0xF, 0xF, 0x0, 0xF}	// expect
		),
		make_tuple(
			(char*)"dst_is_in_front_of_src",
			vector<char> {0xF, 0xF, 0x0, 0xF},
			size_t(1),
			vector<char> {0xF, 0x0, 0x0, 0xF}
		),
		make_tuple(
			(char*)"dst_is_in_front_of_src",
			vector<char> {0xF, 0xF, 0x0, 0x1, 0xF},
			size_t(2),
			vector<char> {0xF, 0x0, 0x1, 0x1, 0xF}
		),
		make_tuple(
			(char*)"dst_is_in_front_of_src",
			vector<char> {0xF, 0xF, 0x0, 0x1, 0x2, 0xF},
			size_t(3),
			vector<char> {0xF, 0x0, 0x1, 0x2, 0x2, 0xF}
		),

		make_tuple(
			(char*)"dst_is_behind_src",
							// src, dst
			vector<char> {0xF, 0x0, 0xF, 0xF},	// dstsrc
			size_t(0),							// size
			vector<char> {0xF, 0x0, 0xF, 0xF}	// expect
		),
		make_tuple(
			(char*)"dst_is_behind_src",
			vector<char> {0xF, 0x0, 0xF, 0xF},
			size_t(1),
			vector<char> {0xF, 0x0, 0x0, 0xF}
		),
		make_tuple(
			(char*)"dst_is_behind_src",
			vector<char> {0xF, 0x0, 0x1, 0xF, 0xF},
			size_t(2),
			vector<char> {0xF, 0x0, 0x0, 0x1, 0xF}
		),
		make_tuple(
			(char*)"dst_is_behind_src",
			vector<char> {0xF, 0x0, 0x1, 0x2, 0xF, 0xF},
			size_t(3),
			vector<char> {0xF, 0x0, 0x0, 0x1, 0x2, 0xF}
		)
	)
);

// ------------------------------------------------------------------
// void *memchr(const void *s, int c, size_t n);
// returns a pointer to the first occurrence of c in the first n bytes of s, or NULL if not found
// ------------------------------------------------------------------
class memchr_test : public ::testing::TestWithParam<
			tuple< unsigned char, size_t, void* >
		>
{ 
	public:
		static vector<unsigned char> s;
};

vector<unsigned char> memchr_test::s = vector<unsigned char> {0x0, 0xF, 0xA, 0x5};

TEST_P(memchr_test, _)
{
	unsigned char	c	= std::get<0>(GetParam());
	size_t			n	= std::get<1>(GetParam());
	void*		expect	= std::get<2>(GetParam());
	void* ret;

	// target
	ret = memchr(&memchr_test::s[0], c, n);

	// check
	EXPECT_EQ(expect, ret);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	memchr_test,
	testing::Values(
		// n = 0
		std::make_tuple(
			(unsigned char)0x0,			// c
			(size_t)0,					// n
			(void*)NULL					// expect
		),

		// n = 1
		std::make_tuple(
			(unsigned char)0x0,
			(size_t)1,
			(void*)&memchr_test::s[0]
		),
		std::make_tuple(
			(unsigned char)0xF,
			(size_t)1,
			(void*)NULL
		),

		// n = 2
		std::make_tuple(
			(unsigned char)0x0,
			(size_t)2,
			(void*)&memchr_test::s[0]
		),
		std::make_tuple(
			(unsigned char)0xF,
			(size_t)2,
			(void*)&memchr_test::s[1]
		),
		std::make_tuple(
			(unsigned char)0xA,
			(size_t)2,
			(void*)NULL
		),

		// n = 3
		std::make_tuple(
			(unsigned char)0x0,
			(size_t)3,
			(void*)&memchr_test::s[0]
		),
		std::make_tuple(
			(unsigned char)0xF,
			(size_t)3,
			(void*)&memchr_test::s[1]
		),
		std::make_tuple(
			(unsigned char)0xA,
			(size_t)3,
			(void*)&memchr_test::s[2]
		),
		std::make_tuple(
			(unsigned char)0x5,
			(size_t)3,
			(void*)NULL
		)
	)
);

// ------------------------------------------------------------------
// int memcmp(const void *s1, const void *s2, size_t n);
// compares the first n bytes of two memory areas
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// void *memset(void *, int c, size_t n);
// overwrites a memory area with n copies of c
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strcat(char *dest, const char *src);
// appends the string src to dest
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strncat(char *dest, const char *src, size_t n);
// appends at most n bytes of the string src to dest
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strchr(const char *, int c);
// locates byte c in a string, searching from the beginning
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strrchr(const char *, int c);
// locates byte c in a string, searching from the end
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// int strcmp(const char *, const char *);
// compares two strings lexicographically
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// int strncmp(const char *, const char *, size_t n);
// compares up to the first n bytes of two strings lexicographically
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// int strcoll(const char *, const char *);
// compares two strings using the current locale's collating order
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strcpy(char *dest, const char *src);
// copies a string from one location to another
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strncpy(char *dest, const char *src, size_t n);
// write exactly n bytes to dest, copying from src or add 0's
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strerror(int);
// returns the string representation of an error number e.g. errno (not thread-safe)
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// size_t strlen(const char *);
// finds the length of a C string
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// size_t strspn(const char *, const char *accept);
// determines the length of the maximal initial substring consisting entirely of bytes in accept
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// size_t strcspn(const char *, const char *reject);
// determines the length of the maximal initial substring consisting entirely of bytes not in reject
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strpbrk(const char *, const char *accept);
// finds the first occurrence of any byte in accept
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strstr(const char *haystack, const char *needle);
// finds the first occurrence of the string "needle" in the longer string "haystack"
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// char *strtok(char *, const char * delim);
// parses a string into a sequence of tokens; non-thread safe in the spec, non-reentrant[1]
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// size_t strxfrm(char *dest, const char *src, size_t n);
// transforms src into a collating form, such that the numerical sort order of the transformed string is equivalent to the collating order of src
// ------------------------------------------------------------------

