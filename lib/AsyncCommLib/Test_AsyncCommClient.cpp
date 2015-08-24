extern "C"
{
#include "AsyncCommClient.h"
#include "private.h"
}
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
const unsigned short cSockServerPortNumber = 12345;
const char* cSockServerAddress = "127.0.0.1";

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper class
// ------------------------------------------------------------------
class SockServerConfig;
class SockServer;

class SockServerConfig
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

static void* __SockServer_run(void* arg);

class SockServer
{
	friend void* __SockServer_run(void* arg);

	protected:
		pthread_t thread_;
		int serverSocket_, peerSocket_;

		pthread_mutex_t mutex_;
		pthread_cond_t condvar_;
		bool connected_;

		bool stopRequested_;
		static const long pollingPeriodMicroSec_ = 100;

		void lock(void)
		{
			pthread_mutex_lock(&mutex_);
		}

		void unlock(void)
		{
			pthread_mutex_unlock(&mutex_);
		}

		bool openServerSocket(struct SockServerConfig* config)
		{
			struct sockaddr_in serverAddr;

			if((serverSocket_ = socket(config->family, SOCK_STREAM, 0)) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			config->setSockAddrIn(&serverAddr);
			if(bind(serverSocket_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			if(listen(serverSocket_, 1) == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return false;
			}

			return true;
		}

		void closeServerSocket(void)
		{
			close(serverSocket_);
		}

		void waitForConnect(void)
		{
			fd_set fds;
			struct timeval timeout;
			int result;

			while(! stopRequested_)
			{
				FD_ZERO(&fds);
				FD_SET(serverSocket_, &fds);
				timeout.tv_sec = 0;
				timeout.tv_usec = pollingPeriodMicroSec_;
				
				result = select(serverSocket_ + 1, &fds, NULL, NULL, &timeout);
				if((result > 0) && FD_ISSET(serverSocket_, &fds))
				{
					break;
				}
				else if(result == -1)
				{
					__printErrnoDetails(errno); DEBUGPOINT;
					stopRequested_ = true;
				}
			}
		}

		void establishPeerConnection(void)
		{
			struct sockaddr_in peerAddr;
			socklen_t sizeofPeerAddr = sizeof(peerAddr);

			peerSocket_ = accept(
					serverSocket_,
					(struct sockaddr*)&peerAddr,
					&sizeofPeerAddr
					);
			if(peerSocket_ == -1)
			{
				__printErrnoDetails(errno); DEBUGPOINT;
				return;
			}

			lock();
			connected_ = true;
			pthread_cond_broadcast(&condvar_);
			unlock();
		}

		void communicateWithPeer(void)
		{
			//int numrcv;
			//const int BUFFER_SIZE = 256;
			//char buffer[BUFFER_SIZE];

			while(! stopRequested_) {
				usleep(pollingPeriodMicroSec_);
				//numrcv = recv(peerSocket_, buffer, BUFFER_SIZE, MSG_DONTWAIT); 
				//if(numrcv == 0 || numrcv == -1) {
				//	close(peerSocket_);
				//	DEBUG_POINT("Connection closed\n");
				//	break;
				//}
				//DEBUG_POINT("received: %s\n", buffer);
			}
		}

		void disestablishPeerConnection(void)
		{
			close(peerSocket_);
		}

		void run(void)
		{
			DEBUG_POINT("the server is running...\n");
			while(! stopRequested_)
			{
				waitForConnect();
				if(stopRequested_)
				{
					break;
				}
				DEBUG_POINT("a connection request came in\n");

				establishPeerConnection();
				DEBUG_POINT("a new connection was established\n");

				communicateWithPeer();
				disestablishPeerConnection();
				DEBUG_POINT("the connection was disestablished\n");
			}

			closeServerSocket();
			DEBUG_POINT("the server is stopping...\n");
		}

	public:
		SockServer(void)
			:serverSocket_(0),
			peerSocket_(0),
			mutex_(PTHREAD_MUTEX_INITIALIZER),
			condvar_(PTHREAD_COND_INITIALIZER),
			connected_(false),
			stopRequested_(false)
		{}

		~SockServer(void)
		{
			stop();
			pthread_mutex_destroy(&mutex_);
			pthread_cond_destroy(&condvar_);
		}

		bool start(struct SockServerConfig* config)
		{
			connected_ = false;
			stopRequested_ = false;

			if(! openServerSocket(config))
			{
				DEBUGPOINT;
				return false;
			}
			if(pthread_create(&thread_, NULL, __SockServer_run, this) != 0)
			{
				DEBUGPOINT;
				return false;
			}

			return true;
		}

		void waitForConnected(void)
		{
			lock();
			while(! connected_)
			{
				pthread_cond_wait(&condvar_, &mutex_);
			}
			unlock();
		}

		bool isConnected(void)
		{
			return connected_;
		}

		void stop(void)
		{
			stopRequested_ = true;
			(void)pthread_join(thread_, NULL);
		}
};

static void* __SockServer_run(void* arg)
{
	SockServer* testServer = (SockServer*)arg;

	testServer->run();
	return NULL;
}

// ------------------------------------------------------------------
// Test_AsyncCommClientCallbacks_create
// ------------------------------------------------------------------
class Test_AsyncCommClientCallbacks_create : public ::testing::Test
{ 
	protected:
		AsyncCommClientCallbacks* callbacks;
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClientCallbacks_create, success)
{
	// precondition

	// target
	callbacks = AsyncCommClientCallbacks_create();

	// postcondition
	EXPECT_NE((AsyncCommClientCallbacks*)NULL, callbacks);
}

// ------------------------------------------------------------------
// Test_AsyncCommClient_create
// ------------------------------------------------------------------
class Test_AsyncCommClient_create : public ::testing::Test
{ 
	protected:
		AsyncCommClient* acc;
		AsyncCommClientConfig* accConfig;

		virtual void SetUp()
		{
			accConfig = AsyncCommClientConfig_create();
		}
		virtual void TearDown()
		{
			AsyncCommClientConfig_delete(accConfig);
		}
};

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_zero)
{
	// precondition
	AsyncCommClientConfig_setNumofConcurrentSend(accConfig, 0);

	// target
	acc = AsyncCommClient_create(accConfig);

	// postcondition
	EXPECT_EQ((AsyncCommClient*)NULL, acc);
}

// ------------------------------------------------------------------
TEST_F(Test_AsyncCommClient_create, numofConcurrentSend_is_one)
{
	// precondition
	AsyncCommClientConfig_setNumofConcurrentSend(accConfig, 1);

	// target
	acc = AsyncCommClient_create(accConfig);

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
//		SockServer* testServer;
//		SockServerConfig* config;
//
//		virtual void SetUp(void)
//		{
//			testServer = new SockServer();
//			config = new SockServerConfig();
//			config->family = AF_INET;
//			config->port = cSockServerPortNumber;
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
//			cSockServerAddress,
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
//		SockServer* testServer;
//		SockServerConfig* config;
//
//		virtual void SetUp(void)
//		{
//			testServer = new SockServer();
//			config = new SockServerConfig();
//			config->family = AF_INET;
//			config->port = cSockServerPortNumber;
//			testServer->start(config);
//
//			numofConcurrentSend = 1;
//			self = AsyncCommClient_create(numofConcurrentSend);
//			AsyncCommClient_connectServer(
//				self,
//				eAsyncCommClientAddressFamily_INET,
//				cSockServerAddress,
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

