#ifndef _MOCK_UTIL_H_
#define _MOCK_UTIL_H_
// ----------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::_;
using ::testing::Invoke;

// ----------------------------------------------
class MockUtil
{
	public:
		MOCK_METHOD1(UtilMalloc, void*(size_t));
		MOCK_METHOD1(UtilFree, void(void*));

		MockUtil()
		{
			ON_CALL(*this, UtilMalloc(_))
				.WillByDefault(Invoke(&MockUtil::utilMalloc));
			ON_CALL(*this, UtilFree(_))
				.WillByDefault(Invoke(&MockUtil::utilFree));
		}
		~MockUtil() {}

		static void setInstance(MockUtil* instance);

	private:
		static void* utilMalloc(size_t size)
		{
			return malloc(size);
		}
		static void utilFree(void* ptr)
		{
			free(ptr);
		}
};


// ----------------------------------------------
#endif // _MOCK_UTIL_H_

