#include <AsyncCommClient.hpp>

#include <boost/asio.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

// ------------------------------------------------------------------
// AsyncCommClient
// ------------------------------------------------------------------
bool AsyncCommClient::connect(
	std::string hostname,
	uint16_t port_number
	)
{
	asio::io_service io_service;
	tcp::socket socket(io_service);
	boost::system::error_code ec;
	std::stringstream ss;
	std::string port_number_str;

	tcp::resolver resolver(io_service);
	ss << port_number;
	ss >> port_number_str;
	tcp::resolver::query query(hostname, port_number_str);

	try
	{
		for( tcp::resolver::iterator it = resolver.resolve(query);
				it != tcp::resolver::iterator();
				it++)
		{
			socket.connect(*it);
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;		// FIXME
		return false;
	}
	return true;
}

//#include <vector>
//using namespace std;
//
////#include <stddef.h>
////#include <stdbool.h>
////#include <stdlib.h>
//#include <cstring>
//
//#include <pthread.h>
//
//#include <errno.h>
//
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//
//#include "private.h"
//
//// ------------------------------------------------------------------
//// define
//// ------------------------------------------------------------------
//
//// ------------------------------------------------------------------
//// Helper function
//// ------------------------------------------------------------------
//void __printErrnoDetails(int errnoSv)
//{
//	printf("errno: [%d], strerror: [%s]\n", errnoSv, strerror(errnoSv));
//}
//
//// ------------------------------------------------------------------
//// ACCCallbackResultImpl
//// ------------------------------------------------------------------
//class ACCCallbackResultImpl : public ACCCallbackResult
//{
//	public:
//		ACCCallbackResultImpl(
//				void* context,
//				uint32_t sendId,
//				unsigned char* receive_data,
//				size_t sizeof_receive_data,
//				int result_code
//				);
//		~ACCCallbackResultImpl();
//
//		void* get_context(void);
//		uint32_t get_sendId(void);
//		unsigned char* get_receive_data(void);
//		size_t get_sizeof_receive_data(void);
//		int get_result_code(void);
//
//	protected:
//		void* _context;
//		uint32_t _sendId;
//		unsigned char* _receive_data;
//		size_t _sizeof_receive_data;
//		int _result_code;
//};
//
//// ------------------------------------------------------------------
//ACCCallbackResultImpl::ACCCallbackResultImpl(
//	void* context,
//	uint32_t sendId,
//	unsigned char* receive_data,
//	size_t sizeof_receive_data,
//	int result_code
//	) :
//	_context(context),
//	_sendId(sendId),
//	_receive_data(receive_data),
//	_sizeof_receive_data(sizeof_receive_data),
//	_result_code(result_code)
//{
//}
//
//// ------------------------------------------------------------------
//ACCCallbackResultImpl::~ACCCallbackResultImpl(void) {}
//
//// ------------------------------------------------------------------
//void* ACCCallbackResultImpl::get_context(void) { return _context; }
//
//// ------------------------------------------------------------------
//uint32_t ACCCallbackResultImpl::get_sendId(void) { return _sendId; }
//
//// ------------------------------------------------------------------
//unsigned char* ACCCallbackResultImpl::get_receive_data(void) { return _receive_data; }
//
//// ------------------------------------------------------------------
//size_t ACCCallbackResultImpl::get_sizeof_receive_data(void) { return _sizeof_receive_data; }
//
//// ------------------------------------------------------------------
//int ACCCallbackResultImpl::get_result_code(void) { return _result_code; }
//
//// ------------------------------------------------------------------
//// ACCCallbacksImpl
//// ------------------------------------------------------------------
//class ACCCallbacksImpl : public ACCCallbacks
//{
//	public:
//		ACCCallbacksImpl();
//		~ACCCallbacksImpl();
//
//		void set_NormalCallback(ACCNormalCallback callback, void* context);
//		void exec_NormalCallback(ACCCallbackResult* result);
//
//	protected:
//		struct
//		{
//			ACCNormalCallback callback;
//			void* context;
//		}_normal;
//};
//
//// ------------------------------------------------------------------
//ACCCallbacksImpl::ACCCallbacksImpl(void)
//{
//	this->set_NormalCallback(NULL, NULL);
//}
//
//// ------------------------------------------------------------------
//ACCCallbacksImpl::~ACCCallbacksImpl() {};
//
//// ------------------------------------------------------------------
//void ACCCallbacksImpl::set_NormalCallback(
//	ACCNormalCallback callback,
//	void* context
//	)
//{
//	_normal.callback = callback;
//	_normal.context = context;
//}
//
//// ------------------------------------------------------------------
//void ACCCallbacksImpl::exec_NormalCallback(ACCCallbackResult* result)
//{
//	if(_normal.callback != NULL)
//	{
//		_normal.callback(
//			result
//			);
//	}
//}
//
//// ------------------------------------------------------------------
//// ACCCallbacks
//// ------------------------------------------------------------------
//ACCCallbacks* ACCCallbacks::create(void)
//{
//	return new ACCCallbacksImpl();
//}
//
//// ------------------------------------------------------------------
//// ACCConfigImpl
//// ------------------------------------------------------------------
//class ACCConfigImpl : public ACCConfig
//{
//	public:
//		ACCConfigImpl(void);
//		~ACCConfigImpl(void);
//
//		void set_numof_concurrent_send(uint32_t numof_concurrent_send);
//		uint32_t get_numof_concurrent_send(void);
//
//	protected:
//		uint32_t _numof_concurrent_send;
//};
//
//// ------------------------------------------------------------------
//ACCConfigImpl::ACCConfigImpl(
//	void
//	)
//{
//}
//
//// ------------------------------------------------------------------
//void ACCConfigImpl::set_numof_concurrent_send(
//	uint32_t numof_concurrent_send
//	)
//{
//	_numof_concurrent_send = numof_concurrent_send;
//}
//
//// ------------------------------------------------------------------
//uint32_t ACCConfigImpl::get_numof_concurrent_send(void)
//{
//	return _numof_concurrent_send;
//}
//
//// ------------------------------------------------------------------
//// ACCConfig
//// ------------------------------------------------------------------
//ACCConfig* ACCConfig::create(void)
//{
//	return new ACCConfigImpl();
//}
//
//// ------------------------------------------------------------------
//// ACCRequest
//// ------------------------------------------------------------------
//class ACCRequest
//{
//	public:
//		ACCRequest(void);
//		~ACCRequest(void);
//
//		void set_callbacks(ACCCallbacks* callbacks);
//		uint32_t get_sendId(void);
//
//	protected:
//		uint32_t _sendId;
//		ACCCallbacks* _callbacks;
//};
//
//// ------------------------------------------------------------------
//ACCRequest::ACCRequest(void) :
//	_sendId(0)
//{
//	_callbacks = ACCCallbacks::create();
//}
//
//// ------------------------------------------------------------------
//ACCRequest::~ACCRequest(void)
//{
//}
//
//// ------------------------------------------------------------------
//void ACCRequest::set_callbacks(
//	ACCCallbacks* callbacks
//	)
//{
//	_callbacks = callbacks;
//}
//
//// ------------------------------------------------------------------
//uint32_t ACCRequest::get_sendId(void)
//{
//	return _sendId;
//}
//
//// ------------------------------------------------------------------
//// ACCRequestList
//// ------------------------------------------------------------------
//class ACCRequestList
//{
//	public:
//		ACCRequestList(void);
//		~ACCRequestList(void);
//
//		void enqueue(ACCRequest* request);
//		ACCRequest* dequeue(void);
//		void lock(void);
//		void unlock(void);
//
//	protected:
//		pthread_mutex_t _mutex;
//		vector<ACCRequest*> _requests;
//
//		void _lock(void);
//		void _unlock(void);
//};
//
//// ------------------------------------------------------------------
//ACCRequestList::ACCRequestList(void) :
//	_mutex(PTHREAD_MUTEX_INITIALIZER),
//	_requests(0)
//{
//}
//
//// ------------------------------------------------------------------
//ACCRequestList::~ACCRequestList(void)
//{
//	pthread_mutex_destroy(&_mutex);
//}
//
//// ------------------------------------------------------------------
//void ACCRequestList::enqueue(
//	ACCRequest* request
//	)
//{
//	_lock();
//	_requests.push_back(request);
//	_unlock();
//}
//
//// ------------------------------------------------------------------
//ACCRequest* ACCRequestList::dequeue(void)
//{
//	ACCRequest* request = NULL;
//
//	_lock();
//	if(! _requests.empty())
//	{
//		request = _requests.front();
//		_requests.erase(_requests.begin());
//	}
//	_unlock();
//
//	return request;
//}
//
//// ------------------------------------------------------------------
//void ACCRequestList::lock(void)
//{
//	_lock();
//}
//
//// ------------------------------------------------------------------
//void ACCRequestList::unlock(void)
//{
//	_unlock();
//}
//
//// ------------------------------------------------------------------
//void ACCRequestList::_lock(void)
//{
//	pthread_mutex_lock(&_mutex);
//}
//
//// ------------------------------------------------------------------
//void ACCRequestList::_unlock(void)
//{
//	pthread_mutex_unlock(&_mutex);
//}
//
//// ------------------------------------------------------------------
//// ACCRequestPool
//// ------------------------------------------------------------------
//class ACCRequestPool
//{
//	public:
//		ACCRequestPool(void);
//		~ACCRequestPool(void);
//
//		bool open_pool(uint32_t numof_requests);
//		void close_pool(void);
//		ACCRequest* lease(void);
//		void release(void);
//
//	protected:
//		ACCRequestList _request_list;
//};
//
//// ------------------------------------------------------------------
//ACCRequestPool::ACCRequestPool(
//	) :
//	_request_list()
//{
//}
//
//// ------------------------------------------------------------------
//ACCRequestPool::~ACCRequestPool(
//	)
//{
//	close_pool();
//}
//
//// ------------------------------------------------------------------
//bool ACCRequestPool::open_pool(
//	uint32_t numof_requests
//	)
//{
//	uint32_t num;
//	ACCRequest* request;
//
//	for(num = 0; num < numof_requests; num++)
//	{
//		request = new ACCRequest();
//		if(request == NULL)
//		{
//			goto lError;
//		}
//		_request_list.enqueue(request);
//	}
//	return true;
//
//lError:
//	close_pool();
//	return false;
//}
//
//// ------------------------------------------------------------------
//void ACCRequestPool::close_pool(void)
//{
//	ACCRequest* request;
//
//	for(request = _request_list.dequeue();
//			request != NULL; 
//			request = _request_list.dequeue())
//	{
//		delete(request);
//	}
//}
//
//
//// ------------------------------------------------------------------
//// ACCSynchronizer
//// ------------------------------------------------------------------
//typedef bool (*ACCSynchronizerWaitCondition)(void* arg);
//
//bool __ACCSynchronizerWaitConditionDefault(void* arg)
//{
//	(void)arg;
//	return true;
//}
//
//// ------------------------------------------------------------------
//class ACCSynchronizer
//{
//	public:
//		ACCSynchronizer(ACCSynchronizerWaitCondition condition, void* arg);
//		~ACCSynchronizer(void);
//
//		void wait_for_notify(void);
//		void notify(void);
//
//	protected:
//		pthread_mutex_t _mutex;
//		pthread_cond_t _cond;
//		ACCSynchronizerWaitCondition _condition;
//		void* _arg;
//};
//
//// ------------------------------------------------------------------
//ACCSynchronizer::ACCSynchronizer(
//	ACCSynchronizerWaitCondition condition,
//	void* arg
//	) :
//	_mutex(PTHREAD_MUTEX_INITIALIZER),
//	_cond(PTHREAD_COND_INITIALIZER)
//{
//	if(condition == NULL)
//	{
//		_condition = __ACCSynchronizerWaitConditionDefault;
//		_arg = NULL;
//	}
//	else
//	{
//		_condition = condition;
//		_arg = arg;
//	}
//}
//
//// ------------------------------------------------------------------
//ACCSynchronizer::~ACCSynchronizer(void)
//{
//	pthread_mutex_destroy(&_mutex);
//	pthread_cond_destroy(&_cond);
//}
//
//// ------------------------------------------------------------------
//void ACCSynchronizer::wait_for_notify(void)
//{
//	pthread_mutex_lock(&_mutex);
//	while(! _condition(_arg))
//	{
//		pthread_cond_wait(&_cond, &_mutex);
//	}
//	pthread_mutex_unlock(&_mutex);
//}
//
//// ------------------------------------------------------------------
//void ACCSynchronizer::notify(void)
//{
//	pthread_mutex_lock(&_mutex);
//	pthread_cond_broadcast(&_cond);
//	pthread_mutex_unlock(&_mutex);
//}
//
//// ------------------------------------------------------------------
//// ACCServiceWork
//// ------------------------------------------------------------------
//class ACCServiceWork
//{
//	public:
//		ACCServiceWork(void);
//		~ACCServiceWork(void);
//
//		void send(
//			ACCRequestList* send_list,
//			ACCRequestList* receive_list,
//			ACCRequestList* callback_list,
//			int socket_to_send
//			);
//		void receive(
//			ACCRequestList* receive_list,
//			ACCRequestList* callback_list,
//			int socket_to_receive,
//			int socket_to_notify_termination
//			);
//		void callback(
//			ACCRequestList* callback_list,
//			ACCRequestList* requst_pool
//			);
//
//		void execute(
//				ACCRequestList* request_list,
//				//ACCService* successor_service,
//				ACCSynchronizer* synchronizer
//				);
//
//	protected:
//		
//};
//
//// ------------------------------------------------------------------
//void ACCServiceWork::send(
//	ACCRequestList* send_list,
//	ACCRequestList* receive_list,
//	ACCRequestList* callback_list,
//	int socket_to_send
//	)
//{
//	//ACCRequest* request;
//
//	//request = send_list->dequeue();
//	//if(request == NULL)
//	//{
//	//	return;
//	//}
//
//	//receive_list->lock();
//	//if(request->send(socket_to_send))
//	//if(ACCSendReceive::send(socket_to_send, request))
//	//{
//	//	receive_list->enqueue(request);
//	//	receive_list->unlock();
//	//}
//	//else
//	//{
//	//	receive_list->unlock();
//	//	request->mark_as_send_failed();
//	//	callback_list->enqueue(request);
//	//}
//}
//
//// ------------------------------------------------------------------
//void ACCServiceWork::receive(
//	ACCRequestList* receive_list,
//	ACCRequestList* callback_list,
//	int socket_to_receive,
//	int socket_to_notify_termination
//	)
//{
//	//ACCRequest* request;
//	//ACCReceiveData receive_data;
//
//	//if(! ACCSendReceive::receive(socket_to_receive socket_to_notify_termination, &receive_data))
//	//{
//	//	request = receive_list->dequeue();
//	//	if(request != NULL)
//	//	{
//	//		request->mark_as_receive_failed();
//	//		callback_list->enqueue(request);
//	//	}
//	//	return;
//	//}
//
//	//request = receive_list->remove_by_id(receive_data.get_id());
//	//if(request == NULL)
//	//{
//	//	return;
//	//}
//
//	//request->set_receive_data(&receive_data);
//	//callback_list->enqueue(request);
//}
//
//// ------------------------------------------------------------------
//void ACCServiceWork::callback(
//	ACCRequestList* callback_list,
//	ACCRequestList* requst_pool
//	)
//{
//	//ACCRequest* request;
//
//	//request = callback_list->dequeue();
//	//if(request == NULL)
//	//{
//	//	return;
//	//}
//
//	//request->callback();
//	//requst_pool->enqueue(request);
//}
//
//// ------------------------------------------------------------------
//// ACCService
//// ------------------------------------------------------------------
//class ACCService
//{
//	public:
//		ACCService(ACCServiceWork* work);
//		~ACCService(void);
//
//		bool start(void);
//		void stop(void);
//
//	protected:
//		bool _stopRequested;
//		ACCServiceWork* _work;
//		pthread_t _thread;
//
//		friend void* __ACCService_main(void* arg);
//};
//
//// ------------------------------------------------------------------
//void* __ACCService_main(
//	void* arg
//	)
//{
//	ACCService* self = (ACCService*)arg;
//
//	//self->_work->execute(NULL, NULL);
//	return NULL;
//}
//
//// ------------------------------------------------------------------
//ACCService::ACCService(
//	ACCServiceWork* work
//	) :
//	_stopRequested(false),
//	_work(work)
//{
//}
//
//// ------------------------------------------------------------------
//ACCService::~ACCService(void)
//{
//}
//
//// ------------------------------------------------------------------
//bool ACCService::start(void)
//{
//	_stopRequested = false;
//	if(pthread_create(&_thread, NULL, __ACCService_main, (void*)this) != 0)
//	{
//		return false;
//	}
//	return true;
//}
//
//// ------------------------------------------------------------------
//void ACCService::stop(void)
//{
//	_stopRequested = true;
//	if(pthread_join(_thread, NULL) != 0)
//	{
//		__printErrnoDetails(errno);
//		CHECKPOINT;
//	}
//}
//
//// ------------------------------------------------------------------
//// AsyncCommClientImpl
//// ------------------------------------------------------------------
//class AsyncCommClientImpl : public AsyncCommClient
//{
//	public:
//		AsyncCommClientImpl(ACCConfig* config);
//		~AsyncCommClientImpl();
//
//		bool connectServer(
//				int family,
//				const char* serverName,
//				uint16_t port,
//				const char* unixDomainName
//				);
//		bool send(
//				ACCCallbacks* callbacks,
//				unsigned char* sendData,
//				size_t sizeofSendData,
//				uint32_t* sendId
//				);
//		bool cancel(uint32_t sendId);
//		void disconnectServer();
//
//	protected:
//};
//
//// ------------------------------------------------------------------
//AsyncCommClientImpl::AsyncCommClientImpl(
//	ACCConfig* config
//	)
//{
//}
//
//// ------------------------------------------------------------------
//bool AsyncCommClientImpl::connectServer(
//	int family,
//	const char* serverName,
//	uint16_t port,
//	const char* unixDomainName
//	)
//{
//	//(void)family;
//	//(void)unixDomainName;
//	//{
//	//	struct sockaddr_in serverAddr;
//
//	//	memset(&serverAddr, 0, sizeof(serverAddr));
//	//	serverAddr.sin_port = htons(port);
//	//	serverAddr.sin_family = AF_INET;
//	//	serverAddr.sin_addr.s_addr = inet_addr(serverName);
//
//	//	self->socket = socket(AF_INET, SOCK_STREAM, 0);
//	//	if(self->socket == -1)
//	//	{
//	//		__printErrnoDetails(errno); DEBUGPOINT;
//	//		return false;
//	//	}
//
//	//	if(connect(self->socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
//	//	{
//	//		__printErrnoDetails(errno); DEBUGPOINT;
//	//		return false;
//	//	}
//	//}
//
//	return true;
//}
//
//// ------------------------------------------------------------------
//void AsyncCommClientImpl::disconnectServer(void)
//{
//	//close(self->socket);
//}
//
//// ------------------------------------------------------------------
//bool AsyncCommClientImpl::send(
//	ACCCallbacks* callbacks,
//	unsigned char* sendData,
//	size_t sizeofSendData,
//	uint32_t* sendId
//	)
//{
//	ACCRequest* request;
//
//	//request = ACCRequestPool_getNewRequest(&(self->pool));
//	//if(request == NULL)
//	//{
//	//	return false;
//	//}
//	//ACCRequest_setCallbacks(request, callbacs);
//	//*sendId = ACCRequest_getSendId(request);
//
//	//ACCService_request(&(self->sendService));
//	return true;
//}
//
//// ------------------------------------------------------------------
//bool AsyncCommClientImpl::cancel(
//	uint32_t sendId
//	)
//{
//	return false;
//}
//
