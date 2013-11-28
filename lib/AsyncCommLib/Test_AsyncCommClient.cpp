extern "C"
{
#include "AsyncCommClient.h"
}

#include "gtest/gtest.h"

// ------------------------------------------------------------------
// Test_AsyncCommClientCallbacks_create
// ------------------------------------------------------------------
class Test_AsyncCommClientCallbacks_create : public ::testing::Test
{ 
	protected:
		AsyncCommClientCallbacks* self;
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClientCallbacks_create, success)
{
	// precondition

	// target
	self = AsyncCommClientCallbacks_create();

	// postcondition
	EXPECT_NE((AsyncCommClientCallbacks*)NULL, self);
}

// ------------------------------------------------------------------
// Test_AsyncCommClientCallbacks_delete
// ------------------------------------------------------------------
class Test_AsyncCommClientCallbacks_delete : public Test_AsyncCommClientCallbacks_create
{ 
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClientCallbacks_delete, self_is_null)
{
	// precondition
	EXPECT_EQ((AsyncCommClientCallbacks*)NULL, self = NULL);

	// target
	AsyncCommClientCallbacks_delete(self);

	// postcondition
	SUCCEED();
}

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClientCallbacks_delete, self_is_not_null)
{
	// precondition
	self = AsyncCommClientCallbacks_create();
	EXPECT_NE((AsyncCommClientCallbacks*)NULL, self);

	// target
	AsyncCommClientCallbacks_delete(self);

	// postcondition
	SUCCEED();
}

// ------------------------------------------------------------------
// Test_AsyncCommClient_create
// ------------------------------------------------------------------
class Test_AsyncCommClient_create : public ::testing::Test
{ 
	protected:
		AsyncCommClient* self;
		int numofConcurrentSend;
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_zero)
{
	// precondition
	EXPECT_EQ(0, numofConcurrentSend = 0);

	// target
	self = AsyncCommClient_create(numofConcurrentSend);

	// postcondition
	EXPECT_EQ((AsyncCommClient*)NULL, self);
}

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_one)
{
	// precondition
	EXPECT_EQ(1, numofConcurrentSend = 1);

	// target
	self = AsyncCommClient_create(numofConcurrentSend);

	// postcondition
	EXPECT_NE((AsyncCommClient*)NULL, self);
}

// ------------------------------------------------------------------
// Test_AsyncCommClient_delete
// ------------------------------------------------------------------
class Test_AsyncCommClient_delete : public Test_AsyncCommClient_create
{ 
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_delete, self_is_null)
{
	// precondition
	EXPECT_EQ((AsyncCommClient*)NULL, self = NULL);

	// target
	AsyncCommClient_delete(self);

	// postcondition
	SUCCEED();
}

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_delete, self_is_not_null)
{
	// precondition
	self = AsyncCommClient_create(1);
	EXPECT_NE((AsyncCommClient*)NULL, self);

	// target
	AsyncCommClient_delete(self);

	// postcondition
	SUCCEED();
}

