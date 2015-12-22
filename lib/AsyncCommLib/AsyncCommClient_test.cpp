
#include <AsyncCommClient.hpp>

#include <gtest/gtest.h>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

// ------------------------------------------------------------------
// AsyncCommClient_test
// ------------------------------------------------------------------
class AsyncCommClient_connect_test : public ::testing::TestWithParam<
			std::tuple< std::string, bool >
		>
{ 
	protected:
		virtual void SetUp() {}
		virtual void TearDown() {}
};

TEST_P(AsyncCommClient_connect_test, _)
{
	AsyncCommClient acc_;
	asio::io_service io_service_;
	uint16_t server_port_ = 10000;
	bool server_connected_ = false;
	std::string hostname	= std::get<0>(GetParam());
	bool expect_result		= std::get<1>(GetParam());
	bool result;

	// precondition
	tcp::acceptor server(io_service_, tcp::endpoint(tcp::v4(), server_port_));
	tcp::socket socket(io_service_);
	server.async_accept(
		socket,
		[&](boost::system::error_code ec)
		{
			if (ec != boost::system::errc::success)
			{
				std::cerr << ec.message() << std::endl;
				return;
			}
			server_connected_ = true;
		});

	// target
	result = acc_.connect(hostname, uint16_t(server_port_));

	// postcondition
	io_service_.poll();
	EXPECT_EQ(expect_result, result);
	EXPECT_EQ(result, server_connected_);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	AsyncCommClient_connect_test,
	testing::Values(
		// normal case
		make_tuple(
			std::string("127.0.0.1"),	// hostname
			bool(true)					// expect_result
		),
		make_tuple(
			std::string("localhost"),
			bool(true)
		),
		// error case
		make_tuple(
			std::string("999.999.999.999"),
			bool(false)
		),
		make_tuple(
			std::string("localhhhh"),
			bool(false)
		)
	)
);

class EchoServer
{
protected:
	asio::io_service io_service_;
	uint16_t server_port_;
	tcp::acceptor server_;
	tcp::socket socket_;
	bool server_connected_;

	EchoServer(void) :
		server_port_(10000),
		server_(io_service_, tcp::endpoint(tcp::v4(), server_port_)),
		socket_(io_service_),
		server_connected_(false)
	{}
	EchoServer(asio::io_service io_service);
	~EchoServer();

	void start_accept(void)
	{
		server_.async_accept(
			socket_,
			[&](boost::system::error_code ec)
			{
				if (ec != boost::system::errc::success)
				{
					std::cerr << ec.message() << std::endl;
					return;
				}
				server_connected_ = true;
			});
	}
};

class AsyncCommClient_disconnect_test : public ::testing::TestWithParam<
			std::tuple< std::string, bool >
		>
{ 
	protected:
		virtual void SetUp() {}
		virtual void TearDown() {}
};

TEST_P(AsyncCommClient_disconnect_test, _)
{
	AsyncCommClient acc_;
	asio::io_service io_service_;
	uint16_t server_port_ = 10000;
	bool server_connected_ = false;
	std::string hostname	= std::get<0>(GetParam());
	bool expect_result		= std::get<1>(GetParam());
	bool result;

	// precondition
	tcp::acceptor server(io_service_, tcp::endpoint(tcp::v4(), server_port_));
	tcp::socket socket(io_service_);
	server.async_accept(
		socket,
		[&](boost::system::error_code ec)
		{
			if (ec != boost::system::errc::success)
			{
				std::cerr << ec.message() << std::endl;
				return;
			}
			server_connected_ = true;
		});

	// target
	result = acc_.connect(hostname, uint16_t(server_port_));

	// postcondition
	io_service_.poll();
	EXPECT_EQ(expect_result, result);
	EXPECT_EQ(result, server_connected_);
}

INSTANTIATE_TEST_CASE_P(
	parameterized_instance,
	AsyncCommClient_disconnect_test,
	testing::Values(
		// normal case
		make_tuple(
			std::string("127.0.0.1"),	// hostname
			bool(true)					// expect_result
		),
		make_tuple(
			std::string("localhost"),
			bool(true)
		),
		// error case
		make_tuple(
			std::string("999.999.999.999"),
			bool(false)
		),
		make_tuple(
			std::string("localhhhh"),
			bool(false)
		)
	)
);

//TEST_F(AsyncCommClient_test, connect)
//{
//	// target
//	//acc.connect(std::string("localhost"), uint16_t(10000));
//}

//extern "C"
//{
//#include "AsyncCommClient.h"
//#include "private.h"
//}
//#include "private.h"
//
//#include <sys/select.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//
//#include <errno.h>
//
//#include <pthread.h>
//
//using std::vector;
//
//
//// ------------------------------------------------------------------
//// Constant
//// ------------------------------------------------------------------
//const unsigned short cSockServerPortNumber = 12345;
//const char* cSockServerAddress = "127.0.0.1";
//
//// ------------------------------------------------------------------
//// Helper function
//// ------------------------------------------------------------------
//
//// ------------------------------------------------------------------
//// Helper class
//// ------------------------------------------------------------------
////class SockServerConfig;
////class SockServer;
////
////class SockServerConfig
////{
////	public:
////		SockServerConfig(int family, unsigned short port) :
////			family_(family),
////			port_(port)
////		{}
////		struct sockaddr_in getSockAddrIn(void)
////		{
////			struct sockaddr_in sockaddr_in;
////
////			sockaddr_in->sin_family = family_;
////			sockaddr_in->sin_port_ = htons(port_);
////			sockaddr_in->sin_addr.s_addr = htonl(INADDR_ANY);
////			return sockaddr_in;
////		}
////
////	private:
////		int family_;
////		unsigned short port_;
////
////		SockServerConfig(void);
////};
////
////typedef (void SockServerThreadRoutine)(void* arg);
////static void* __SockServerThread_run(void* arg);
////
////class SockServerThread
////{
////	private:
////		pthread_t thread_;
////		SockServerThreadRoutine routine_;
////		void* routineArg_;
////		bool requestStart_;
////
////		pthread_mutex_t mutex_;
////		pthread_cond_t cond_;
////
////		static void __lock(void)
////		{
////			pthread_mutex_lock(&mutex_);
////		}
////		static void __unlock(void)
////		{
////			pthread_mutex_unlock(&mutex_);
////		}
////		static void __condwait(void)
////		{
////			pthread_cond_wait(&cond_, &mutex_);
////		}
////		static void __condsignal(void)
////		{
////			pthread_cond_signal(&cond_, &mutex_);
////		}
////	public:
////		static SockServerThread* create(SockServerThreadRoutine* routine, void* routineArg);
////		{
////			SockServerThread* self = new SockServerThread();
////
////			routine_ = routine;
////			routineArg_ = routineArg;
////			requestStart_ = false;
////			mutex_ = PTHREAD_MUTEX_INITIALIZER;
////			cond_ = PTHREAD_COND_INITIALIZER;
////			if(pthread_create(&thread_, NULL, __SockServerThread_run, this) != 0)
////			{
////				delete(self);
////				return NULL;
////			}
////			return self;
////		}
////		static void destroy(SockServerThread* self)
////		{
////			delete(self);
////		}
////
////		bool invoke(void)
////		{
////			__lock();
////			requestStart_ = true;
////			__unlock();
////		}
////
////		void __run(void)
////		{
////			while(! requestStart_)
////			{
////				__condwait();
////			}
////			routine_(routineArg_);
////		}
////}
////
////static void* __SockServerThread_run(void* arg)
////{
////	SockServerThread* self = (SockServerThread)arg;
////
////	self->__run();
////	return NULL;
////}
////
////class SockServer
////{
////	friend void* __SockServer_run(void* arg);
////
////	private::
////		pthread_t thread_;
////		uint32_t maxofListen_;
////		long pollingPeriodMicroSec_ = 100;
////
////		int serverSocket_;
////		std::vector<int> peerSockets_;
////		std::vector<pthread_t> peerSockets_;
////
////		//pthread_mutex_t mutex_;
////		//pthread_cond_t condvar_;
////		//bool connected_;
////
////		//bool stopRequested_;
////
////		void lock(void)
////		{
////			pthread_mutex_lock(&mutex_);
////		}
////
////		void unlock(void)
////		{
////			pthread_mutex_unlock(&mutex_);
////		}
////
////		bool __openServerSocket(const SockServerConfig& config)
////		{
////			struct sockaddr_in serverAddr = config.getSockAddrIn();
////
////			if((serverSocket_ = socket(serverAddr->sin_family, SOCK_STREAM, 0)) == -1)
////			{
////				__printErrnoDetails(errno); DEBUGPOINT;
////				return false;
////			}
////			if(bind(serverSocket_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
////			{
////				__printErrnoDetails(errno); DEBUGPOINT;
////				(void)close(serverSocket_);
////				return false;
////			}
////			if(listen(serverSocket_, 1) == -1)
////			{
////				__printErrnoDetails(errno); DEBUGPOINT;
////				(void)close(serverSocket_);
////				return false;
////			}
////			return true;
////		}
////
////		bool __closeServerSocket(void)
////		{
////			if(close(serverSocket_) != 0)
////			{
////				__printErrnoDetails(errno); DEBUGPOINT;
////				return false;
////			}
////			return true;
////		}
////
////		void __waitForConnect(void)
////		{
////			struct pollfd pollfd;
////			int timeoutMillisec = 100; // TODO: fixed value
////			int result;
////
////			while(! stopRequested_)
////			{
////				pollfd.fd = serverSocket_;
////				pollfd.events = POLLIN;
////
////				result = poll(&pollfd, 1, timeoutMillisec);
////				if(result > 0)
////				{
////					// succeed to poll
////					return;
////				}
////				else if(result == 0)
////				{
////					// timeout
////					continue;
////				}
////				else
////				{
////					// error
////					__printErrnoDetails(errno); DEBUGPOINT;
////					stopRequested_ = true;
////				}
////			}
////		}
////
////		void establishPeerConnection(void)
////		{
////			struct sockaddr_in peerAddr;
////			socklen_t sizeofPeerAddr = sizeof(peerAddr);
////
////			peerSocket_ = accept(
////					serverSocket_,
////					(struct sockaddr*)&peerAddr,
////					&sizeofPeerAddr
////					);
////			if(peerSocket_ == -1)
////			{
////				__printErrnoDetails(errno); DEBUGPOINT;
////				return;
////			}
////
////			lock();
////			connected_ = true;
////			pthread_cond_broadcast(&condvar_);
////			unlock();
////		}
////
////		void communicateWithPeer(void)
////		{
////			//int numrcv;
////			//const int BUFFER_SIZE = 256;
////			//char buffer[BUFFER_SIZE];
////
////			while(! stopRequested_) {
////				usleep(pollingPeriodMicroSec_);
////				//numrcv = recv(peerSocket_, buffer, BUFFER_SIZE, MSG_DONTWAIT); 
////				//if(numrcv == 0 || numrcv == -1) {
////				//	close(peerSocket_);
////				//	DEBUG_POINT("Connection closed\n");
////				//	break;
////				//}
////				//DEBUG_POINT("received: %s\n", buffer);
////			}
////		}
////
////		void disestablishPeerConnection(void)
////		{
////			close(peerSocket_);
////		}
////
////		void __run(void)
////		{
////			DEBUG_POINT("the server is running...\n");
////			while(! stopRequested_)
////			{
////				__waitForConnect();
////				if(stopRequested_)
////				{
////					break;
////				}
////				DEBUG_POINT("a connection request came in\n");
////
////				establishPeerConnection();
////				DEBUG_POINT("a new connection was established\n");
////
////				communicateWithPeer();
////				disestablishPeerConnection();
////				DEBUG_POINT("the connection was disestablished\n");
////			}
////
////			DEBUG_POINT("the server is stopping...\n");
////		}
////
////	private:
////		SockServer(void) :
////			serverSocket_(0),
////			peerSocket_(0)
////			//mutex_(PTHREAD_MUTEX_INITIALIZER),
////			//condvar_(PTHREAD_COND_INITIALIZER),
////			//connected_(false),
////			//stopRequested_(false)
////		{}
////		~SockServer(void)
////		{
////		}
////
////		bool __initialize(const SockServerConfig& config)
////		{
////			if(! __openServerSocket(config))
////			{
////				DEBUGPOINT;
////				return false;
////			}
////			if(pthread_create(&thread_, NULL, __SockServer_run, this) != 0)
////			{
////				DEBUGPOINT;
////				(void)__closeServerSocket();
////				return false;
////			}
////			return true;
////		}
////		void __finalize(void)
////		{
////			(void)__closeServerSocket();
////		}
////
////	public:
////		static SockServerConfig* create(const SockServerConfig& config)
////		{
////			SockServerConfig* instance = new SockServer();
////
////			if(! instance.__initialize(config))
////			{
////				return NULL;
////			}
////			return instance;
////		}
////		static void destroy(SockServer* instance)
////		{
////			 instance.__finalize();
////		}
////
////		void stop(void)
////		{
////			stopRequested_ = true;
////			(void)pthread_join(thread_, NULL);
////		}
////};
////
////static void* __SockServer_run(void* arg)
////{
////	SockServer* self = (SockServer*)arg;
////
////	self->__run();
////	return NULL;
////}
//
//// ------------------------------------------------------------------
//// Test_AsyncCommClientCallbacks_create
//// ------------------------------------------------------------------
//class Test_AsyncCommClientCallbacks_create : public ::testing::Test
//{ 
//	protected:
//		AsyncCommClientCallbacks* callbacks;
//};
//
//// ------------------------------------------------------------------
//TEST_F(Test_AsyncCommClientCallbacks_create, success)
//{
//	// precondition
//
//	// target
//	callbacks = AsyncCommClientCallbacks_create();
//
//	// postcondition
//	EXPECT_NE((AsyncCommClientCallbacks*)NULL, callbacks);
//}
//
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

