
#include "MemoryReflector.hpp"
#include "private.hpp"

#include "gtest/gtest.h"

using std::tuple;


// ------------------------------------------------------------------
// Constant
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper class
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// MemoryReflector_create_test
// ------------------------------------------------------------------
class MemoryReflector_create_test : public ::testing::Test
{ 
	protected:
		MemoryReflector* reflector;
};

// ------------------------------------------------------------------
TEST_F(MemoryReflector_create_test, success)
{
	// precondition

	// target
	reflector = MemoryReflector::create();

	// postcondition
	EXPECT_NE((MemoryReflector*)NULL, reflector);
}

// ------------------------------------------------------------------
// MemoryReflector_destroy_test
// ------------------------------------------------------------------
class MemoryReflector_destroy_test : public MemoryReflector_create_test
{ 
};

// ------------------------------------------------------------------
TEST_F(MemoryReflector_destroy_test, success)
{
	// precondition
	reflector = MemoryReflector::create();
	EXPECT_NE((MemoryReflector*)NULL, reflector);

	// target
	MemoryReflector::destroy(reflector);

	// postcondition
	// expect not segmentation fault
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_destroy_test, invalid_arguments)
{
	// precondition

	// target
	MemoryReflector::destroy(nullptr);
	MemoryReflector::destroy(NULL);
	MemoryReflector::destroy(0);

	// postcondition
	// expect not segmentation fault
}

// ------------------------------------------------------------------
// MemoryReflector_enregister_test
// ------------------------------------------------------------------
class MemoryReflector_enregister_test : public MemoryReflector_destroy_test
{ 
	protected:
		ReflectionAttribute reflectionAttr;
		bool ret;

		virtual void SetUp()
		{
			reflector = MemoryReflector::create();
			EXPECT_NE((MemoryReflector*)NULL, reflector);
		}
		virtual void TearDown()
		{
			MemoryReflector::destroy(reflector);
		}
};

// ------------------------------------------------------------------
TEST_F(MemoryReflector_enregister_test, success)
{
	// precondition
	reflectionAttr.srcAddr = alloca(1);
	reflectionAttr.dstAddr = alloca(1);
	reflectionAttr.sizeofBytes = 1;

	// target
	ret = reflector->enregister(reflectionAttr);

	// postcondition
	EXPECT_TRUE(ret);
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_enregister_test, invalid_arguments)
{
	// precondition
	ret = false;
	reflectionAttr.srcAddr = nullptr;
	reflectionAttr.dstAddr = nullptr;
	reflectionAttr.sizeofBytes = 0;

	// target
	ret |= reflector->enregister(reflectionAttr);

	// postcondition
	EXPECT_FALSE(ret);
}

// ------------------------------------------------------------------
// MemoryReflector_reflectSrc_test
// ------------------------------------------------------------------
class MemoryReflector_reflect_test : public MemoryReflector_enregister_test
{ 
	protected:
		ReflectionAttribute reflectionAttr;
		bool ret;

		virtual void SetUp()
		{
			reflector = MemoryReflector::create();
			EXPECT_NE((MemoryReflector*)NULL, reflector);
		}
		virtual void TearDown()
		{
			MemoryReflector::destroy(reflector);
		}
};

// ------------------------------------------------------------------
TEST_F(MemoryReflector_reflect_test, success)
{
	int src = 1;
	int dst = 0;

	// precondition
	reflectionAttr.srcAddr = &src;
	reflectionAttr.dstAddr = &dst;
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = sizeof(int);
	EXPECT_TRUE(reflector->enregister(reflectionAttr));
	EXPECT_TRUE(src != dst);

	// target
	reflector->reflectSrc(pthread_self());
	reflector->reflectDst(pthread_self());

	// postcondition
	EXPECT_EQ(src, dst);
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_reflect_test, reflect_src_only)
{
	int src;
	int dst;

	// precondition
	reflectionAttr.srcAddr = &src;
	reflectionAttr.dstAddr = &dst;
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = sizeof(int);
	EXPECT_TRUE(reflector->enregister(reflectionAttr));
	src = 1;
	dst = 0;

	// target
	reflector->reflectSrc(pthread_self());

	// postcondition
	EXPECT_EQ(1, src);
	EXPECT_EQ(0, dst);
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_reflect_test, reflect_dst_only)
{
	int src = 1;
	int dst = 0;

	// precondition
	reflectionAttr.srcAddr = &src;
	reflectionAttr.dstAddr = &dst;
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = sizeof(int);
	EXPECT_TRUE(reflector->enregister(reflectionAttr));
	EXPECT_TRUE(src != dst);

	// target
	reflector->reflectDst(pthread_self());

	// postcondition
	EXPECT_NE(src, dst);
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_reflect_test, reflect_src_2_times_then_dst)
{
	int src;
	int dst;

	// precondition
	reflectionAttr.srcAddr = &src;
	reflectionAttr.dstAddr = &dst;
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = sizeof(int);
	EXPECT_TRUE(reflector->enregister(reflectionAttr));
	src = 1;
	dst = 0;
	reflector->reflectSrc(pthread_self());
	EXPECT_EQ(0, dst);

	// target
	src = 2;
	reflector->reflectSrc(pthread_self());
	reflector->reflectDst(pthread_self());

	// postcondition
	EXPECT_EQ(2, dst);
}

// ------------------------------------------------------------------
TEST_F(MemoryReflector_reflect_test, reflect_src_then_dst_2_times)
{
	int src;
	int dst;

	// precondition
	reflectionAttr.srcAddr = &src;
	reflectionAttr.dstAddr = &dst;
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = sizeof(int);
	EXPECT_TRUE(reflector->enregister(reflectionAttr));
	src = 1;
	dst = 0;
	reflector->reflectSrc(pthread_self());
	reflector->reflectDst(pthread_self());
	EXPECT_EQ(1, dst);

	// target
	reflector->reflectDst(pthread_self());

	// postcondition
	EXPECT_EQ(1, dst);
}

// ------------------------------------------------------------------
class MemoryReflector_reflect_multibytes_test : public ::testing::TestWithParam<
			size_t
		>
{ 
};

// ------------------------------------------------------------------
TEST_P(MemoryReflector_reflect_multibytes_test, _)
{
	MemoryReflector* reflector = MemoryReflector::create();
	ReflectionAttribute reflectionAttr;
	std::array<uint8_t, 32>src;
	std::array<uint8_t, 32>dst;

	// precondition
	src.fill(0xFF);
	dst.fill(0x00);
	reflectionAttr.srcAddr = src.data();
	reflectionAttr.dstAddr = dst.data();
	reflectionAttr.srcContext = pthread_self();
	reflectionAttr.dstContext = pthread_self();
	reflectionAttr.sizeofBytes = GetParam();
	EXPECT_TRUE(reflector->enregister(reflectionAttr));

	// target
	reflector->reflectSrc(pthread_self());
	reflector->reflectDst(pthread_self());

	// postcondition
	size_t i;
	for( i = 0; i < reflectionAttr.sizeofBytes; i++ )
	{
		EXPECT_EQ(0xFF, dst[i]);
	}
	for( ; i < dst.size(); i++ )
	{
		EXPECT_EQ(0x00, dst[i]);
	}
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	MemoryReflector_reflect_multibytes_test,
	::testing::Range(
		(size_t)1, (size_t)17
	)
);

