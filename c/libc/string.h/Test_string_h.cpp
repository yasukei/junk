// reference: http://en.wikibooks.org/wiki/C_Programming/C_Reference/string.h

#include <stdio.h>
#include <stdarg.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C"
{
#include <string.h>
}

// void *memcpy(void *dest, const void *src, size_t n);
// copies n bytes between two memory areas; if there is overlap, the behavior is undefined
class memcpy_test : public ::testing::Test
{ 
	protected:
		virtual void SetUp()
		{
			src = NULL;
			dst = NULL;
			size = 0;
		}
		virtual void TearDown()
		{
		}

		char* src;
		char* dst;
		size_t size;
		void* ret;

		char c1[8], c2[8];
		int i1, i2;
};

// TODO: want to arrange these tests as parameterized tests
TEST_F(memcpy_test, invalid_arguments_001)
{
	// prepare
	src = NULL;
	dst = NULL;
	size = 0;

	// target
	ret = memcpy(dst, src, size);

	// check
	EXPECT_EQ(dst, ret);
}

TEST_F(memcpy_test, invalid_arguments_002)
{
	// prepare
	src = c1;
	dst = NULL;
	size = 0;

	// target
	ret = memcpy(dst, src, size);

	// check
	EXPECT_EQ(dst, ret);
}

TEST_F(memcpy_test, invalid_arguments_003)
{
	// prepare
	src = c1;
	dst = c2;
	size = 0;

	// target
	ret = memcpy(dst, src, size);

	// check
	EXPECT_EQ(dst, ret);
}

TEST_F(memcpy_test, invalid_arguments_004)
{
	// prepare
	src = c1;
	dst = c2;
	size = 2;

	// target
	ret = memcpy(dst, src, size);

	// check
	EXPECT_EQ(dst, ret);
	for(int i = 0; i < size; i++)
	{
		EXPECT_EQ(src[i], dst[i]);
	}
}

//// copies n bytes between two memory areas; unlike with memcpy the areas may overlap
//void *memmove(void *dest, const void *src, size_t n);
//// returns a pointer to the first occurrence of c in the first n bytes of s, or NULL if not found
//void *memchr(const void *s, int c, size_t n);
//// compares the first n bytes of two memory areas
//int memcmp(const void *s1, const void *s2, size_t n);
//// overwrites a memory area with n copies of c
//void *memset(void *, int c, size_t n);
//// appends the string src to dest
//char *strcat(char *dest, const char *src);
//// appends at most n bytes of the string src to dest
//char *strncat(char *dest, const char *src, size_t n);
//// locates byte c in a string, searching from the beginning
//char *strchr(const char *, int c);
//// locates byte c in a string, searching from the end
//char *strrchr(const char *, int c);
//// compares two strings lexicographically
//int strcmp(const char *, const char *);
//// compares up to the first n bytes of two strings lexicographically
//int strncmp(const char *, const char *, size_t n);
//// compares two strings using the current locale's collating order
//int strcoll(const char *, const char *);
//// copies a string from one location to another
//char *strcpy(char *dest, const char *src);
//// write exactly n bytes to dest, copying from src or add 0's
//char *strncpy(char *dest, const char *src, size_t n);
//// returns the string representation of an error number e.g. errno (not thread-safe)
//char *strerror(int);
//// finds the length of a C string
//size_t strlen(const char *);
//// determines the length of the maximal initial substring consisting entirely of bytes in accept
//size_t strspn(const char *, const char *accept);
//// determines the length of the maximal initial substring consisting entirely of bytes not in reject
//size_t strcspn(const char *, const char *reject);
//// finds the first occurrence of any byte in accept
//char *strpbrk(const char *, const char *accept);
//// finds the first occurrence of the string "needle" in the longer string "haystack"
//char *strstr(const char *haystack, const char *needle);
//// parses a string into a sequence of tokens; non-thread safe in the spec, non-reentrant[1]
//char *strtok(char *, const char * delim);
//// transforms src into a collating form, such that the numerical sort order of the transformed string is equivalent to the collating order of src
//size_t strxfrm(char *dest, const char *src, size_t n);
