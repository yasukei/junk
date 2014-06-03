//extern "C"
//{
//#include "AsyncCommClient.h"
//#include "private.h"
//}
#include "AsyncCommClient.hpp"
#include "private.h"

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <pthread.h>

#include "gtest/gtest.h"
using std::vector;


// ------------------------------------------------------------------
// Constant
// ------------------------------------------------------------------
const unsigned short cTestServerPortNumber = 12345;
const char* cTestServerAddress = "127.0.0.1";

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper class
// ------------------------------------------------------------------
class TestServerConfig;
class TestServer;

class TestServerConfig
{
	public:
		int family;
		unsigned short port;

		void setSockAddrIn(struct sockaddr_in* addr)
		{
			memset(addr, 0, sizeof(struct sockaddr_in ));
			addr->sin_family = this->family;
			addr->sin_port = htons(this->port);
			addr->sin_addr.s_addr = htonl(INADDR_ANY);
		}

		void display(FILE* fp)
		{
			fprintf(fp, "this:           [%p]\n", this);
			fprintf(fp, "this->family:   [%d]\n", this->family);
			fprintf(fp, "     AF_INET:   [%d]\n", AF_INET);
			fprintf(fp, "     AF_UNIX:   [%d]\n", AF_UNIX);
			fprintf(fp, "this->port:     [%d]\n", this->port);
		}
};

static void* __TestServer_run(void* arg);

class TestServer
{
	friend void* __TestServer_run(void* arg);

	protected:
		pthread_t thread;
		int serverSocket, peerSocket;

		pthread_mutex_t mutex;
		pthread_cond_t condvar;
		bool connected;

		bool stopRequested;
		static const long pollingPeriodMicroSec = 100;

		void lock(void)
		{
			pthread_mutex_lock(&this->mutex);
		}

		void unlock(void)
		{
			pthread_mutex_unlock(&this->mutex);
		}

		bool openServerSocket(struct TestServerConfig* config)
		{
			struct sockaddr_in serverAddr;

			if((this->serverSocket = socket(config->family, SOCK_STREAM, 0)) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			config->setSockAddrIn(&serverAddr);
			if(bind(this->serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			if(listen(this->serverSocket, 1) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			return true;
		}

		void closeServerSocket(void)
		{
			close(this->serverSocket);
		}

		void waitForConnect(void)
		{
			fd_set fds;
			struct timeval timeout;
			int result;

			while(! this->stopRequested)
			{
				FD_ZERO(&fds);
				FD_SET(this->serverSocket, &fds);
				timeout.tv_sec = 0;
				timeout.tv_usec = this->pollingPeriodMicroSec;
				
				result = select(this->serverSocket + 1, &fds, NULL, NULL, &timeout);
				if((result > 0) && FD_ISSET(this->serverSocket, &fds))
				{
					break;
				}
				else if(result == -1)
				{
					__printErrnoDetails(errno); DEBUGPOINT;
					this->stopRequested = true;
				}
			}
		}

		void establishPeerConnection(void)
		{
			struct sockaddr_in peerAddr;
			socklen_t sizeofPeerAddr = sizeof(peerAddr);

			this->peerSocket = accept(
					this->serverSocket,
					(struct sockaddr*)&peerAddr,
					&sizeofPeerAddr
					);
			if(this->peerSocket == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return;
			}

			this->lock();
			this->connected = true;
			pthread_cond_broadcast(&this->condvar);
			this->unlock();
		}

		void communicateWithPeer(void)
		{
			//int numrcv;
			//const int BUFFER_SIZE = 256;
			//char buffer[BUFFER_SIZE];

			while(! this->stopRequested) {
				usleep(this->pollingPeriodMicroSec);
				//numrcv = recv(this->peerSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT); 
				//if(numrcv == 0 || numrcv == -1) {
				//	close(peerSocket);
				//	DEBUG_POINT("Connection closed\n");
				//	break;
				//}
				//DEBUG_POINT("received: %s\n", buffer);
			}
		}

		void disestablishPeerConnection(void)
		{
			close(this->peerSocket);
		}

		void run(void)
		{
			DEBUG_POINT("the server is running...\n");
			while(! this->stopRequested)
			{
				this->waitForConnect();
				if(this->stopRequested)
				{
					break;
				}
				DEBUG_POINT("a connection request came in\n");

				this->establishPeerConnection();
				DEBUG_POINT("a new connection was established\n");

				this->communicateWithPeer();
				this->disestablishPeerConnection();
				DEBUG_POINT("the connection was disestablished\n");
			}

			this->closeServerSocket();
			DEBUG_POINT("the server is stopping...\n");
		}

	public:
		TestServer(void)
			:serverSocket(0),
			peerSocket(0),
			mutex(PTHREAD_MUTEX_INITIALIZER),
			condvar(PTHREAD_COND_INITIALIZER),
			connected(false),
			stopRequested(false)
		{}

		~TestServer(void)
		{
			this->stop();
			pthread_mutex_destroy(&this->mutex);
			pthread_cond_destroy(&this->condvar);
		}

		bool start(struct TestServerConfig* config)
		{
			this->connected = false;
			this->stopRequested = false;

			if(! this->openServerSocket(config))
			{
				DEBUGPOINT;
				return false;
			}
			if(pthread_create(&this->thread, NULL, __TestServer_run, this) != 0)
			{
				DEBUGPOINT;
				return false;
			}

			return true;
		}

		void waitForConnected(void)
		{
			this->lock();
			while(! this->connected)
			{
				pthread_cond_wait(&this->condvar, &this->mutex);
			}
			this->unlock();
		}

		bool isConnected(void)
		{
			return this->connected;
		}

		void stop(void)
		{
			this->stopRequested = true;
			(void)pthread_join(thread, NULL);
		}
};

static void* __TestServer_run(void* arg)
{
	TestServer* testServer = (TestServer*)arg;

	testServer->run();
	return NULL;
}

// ------------------------------------------------------------------
// Test_AsyncCommClientCallbacks_create
// ------------------------------------------------------------------
class Test_AsyncCommClientCallbacks_create : public ::testing::Test
{ 
	protected:
		ACCCallbacks* callbacks;
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClientCallbacks_create, success)
{
	// precondition

	// target
	callbacks = ACCCallbacks::create();

	// postcondition
	EXPECT_NE((ACCCallbacks*)NULL, callbacks);
}

// ------------------------------------------------------------------
// Test_AsyncCommClient_create
// ------------------------------------------------------------------
class Test_AsyncCommClient_create : public ::testing::Test
{ 
	protected:
		AsyncCommClient* acc;
		ACCConfig* config;

		virtual void SetUp()
		{
			config = ACCConfig::create();
		}
		virtual void TearDown()
		{
			delete(config);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_zero)
{
	// precondition
	config->set_numof_concurrent_send(0);

	// target
	acc = AsyncCommClient::create(config);

	// postcondition
	EXPECT_EQ((AsyncCommClient*)NULL, acc);
}

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_one)
{
	// precondition
	config->set_numof_concurrent_send(1);

	// target
	acc = AsyncCommClient::create(config);

	// postcondition
	EXPECT_NE((AsyncCommClient*)NULL, acc);
}

//// ------------------------------------------------------------------
//// Test_AsyncCommClient_connectServer
//// ------------------------------------------------------------------
//class Test_AsyncCommClient_connectServer : public ::testing::Test
//{ 
//	protected:
//		AsyncCommClient* self;
//		int numofConcurrentSend;
//		bool ret;
//		TestServer* testServer;
//		TestServerConfig* config;
//
//		virtual void SetUp(void)
//		{
//			testServer = new TestServer();
//			config = new TestServerConfig();
//			config->family = AF_INET;
//			config->port = cTestServerPortNumber;
//		}
//
//		virtual void TearDown(void)
//		{
//			testServer->stop();
//			delete config;
//			delete testServer;
//		}
//};
//
//// ------------------------------------------------------------------
//TEST_F(Test_AsyncCommClient_connectServer, connectByINET)
//{
//	// precondition
//	self = AsyncCommClient_create(1);
//	ASSERT_NE((AsyncCommClient*)NULL, self);
//	ASSERT_TRUE(testServer->start(config));
//
//	// target
//	ret = AsyncCommClient_connectServer(
//			self,
//			eAsyncCommClientAddressFamily_INET,
//			cTestServerAddress,
//			config->port,
//			""
//			);
//
//	// postcondition
//	testServer->waitForConnected();
//	EXPECT_TRUE(ret);
//	EXPECT_TRUE(testServer->isConnected());
//	AsyncCommClient_disconnectServer(self);
//}
//
//// ------------------------------------------------------------------
//// Test_AsyncCommClient_send
//// ------------------------------------------------------------------
//
//// ------------------------------------------------------------------
//class NormalCallbackResult
//{
//	public:
//		uint32_t sendId;
//		int result;
//		vector<unsigned char> receiveData;
//};
//
//// ------------------------------------------------------------------
//static void __normalCallback(
//	void* context,
//	uint32_t sendId,
//	unsigned char* receiveData,
//	size_t sizeofReceiveData,
//	int result
//	)
//{
//	NormalCallbackResult* callbackResult = (NormalCallbackResult*)context;
//
//	callbackResult->sendId = sendId;
//	callbackResult->receiveData = vector<unsigned char>(sizeofReceiveData);
//	memcpy(&callbackResult->receiveData[0], receiveData, sizeofReceiveData);
//	callbackResult->result = result;
//}
//
//class Test_AsyncCommClient_send : public ::testing::Test
//{ 
//	protected:
//		AsyncCommClient* self;
//		int numofConcurrentSend;
//		bool ret;
//		ACCCallbacks* callbacks;
//		vector<unsigned char> sendData;
//		uint32_t sendId;
//		NormalCallbackResult* callbackResult;
//		TestServer* testServer;
//		TestServerConfig* config;
//
//		virtual void SetUp(void)
//		{
//			testServer = new TestServer();
//			config = new TestServerConfig();
//			config->family = AF_INET;
//			config->port = cTestServerPortNumber;
//			testServer->start(config);
//
//			numofConcurrentSend = 1;
//			self = AsyncCommClient_create(numofConcurrentSend);
//			AsyncCommClient_connectServer(
//				self,
//				eAsyncCommClientAddressFamily_INET,
//				cTestServerAddress,
//				config->port,
//				""
//				);
//			testServer->waitForConnected();
//
//			callbackResult = new NormalCallbackResult();
//			AsyncCommClientCallbacks_setNormalCallback(
//				callbacks, __normalCallback, callbackResult
//				);
//		}
//
//		virtual void TearDown(void)
//		{
//			AsyncCommClient_disconnectServer(self);
//			delete self;
//
//			testServer->stop();
//			delete config;
//			delete testServer;
//		}
//};
//
//// ------------------------------------------------------------------
//TEST_F(Test_AsyncCommClient_send, callback_is_called)
//{
//	// precondition
//	ASSERT_NE((AsyncCommClient*)NULL, self);
//	ASSERT_TRUE(testServer->isConnected());
//	sendData = vector<unsigned char> {0x00, 0xFF};
//
//	// target
//	ret = AsyncCommClient_send(
//			self,
//			callbacks,
//			(unsigned char*)&sendData[0],
//			(size_t)sendData.size(),
//			&sendId
//			);
//
//	// postcondition
//	EXPECT_TRUE(ret);
//	EXPECT_EQ(sendId, callbackResult->sendId);
//	EXPECT_EQ(eAsyncCommClientResult_success, callbackResult->result);
//	EXPECT_EQ(sendData, callbackResult->receiveData);
//	EXPECT_EQ(sendData.size(), callbackResult->receiveData.size());
//}

