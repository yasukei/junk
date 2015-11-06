//#ifndef ASYNCCOMMCLIENT_HPP_
//#define ASYNCCOMMCLIENT_HPP_
//
//#include <cstdint>
//#include <cstddef>
//
//// ------------------------------------------------------------------
//class ACCCallbackResult
//{
//	public:
//		ACCCallbackResult* create(void);
//		~ACCCallbackResult() {};
//
//		virtual void* get_context(void) = 0;
//		virtual uint32_t get_sendId(void) = 0;
//		virtual unsigned char* get_receive_data(void) = 0;
//		virtual size_t get_sizeof_receive_data(void) = 0;
//		virtual int get_result_code(void) = 0;
//};
//
//// ------------------------------------------------------------------
//typedef void (*ACCNormalCallback)(
//	ACCCallbackResult* result
//	);
//
//// ------------------------------------------------------------------
//class ACCCallbacks
//{
//	public:
//		static ACCCallbacks* create(void);
//		~ACCCallbacks() {};
//
//		virtual void set_NormalCallback(ACCNormalCallback callback, void* context) = 0;
//		virtual void exec_NormalCallback(ACCCallbackResult* result) = 0;
//};
//
//// ------------------------------------------------------------------
//class ACCConfig
//{
//	public:
//		static ACCConfig* create(void);
//		~ACCConfig() {};
//
//		virtual void set_numof_concurrent_send(uint32_t numof_concurrent_send) = 0;
//		virtual uint32_t get_numof_concurrent_send(void) = 0;
//};
//
//// ------------------------------------------------------------------
//class AsyncCommClient
//{
//	public:
//		static AsyncCommClient* create(ACCConfig* config);
//		~AsyncCommClient() {};
//
//		virtual bool connectServer(
//					int family,
//					const char* serverName,
//					uint16_t port,
//					const char* unixDomainName
//					) = 0;
//		virtual bool send(
//					ACCCallbacks* callbacks,
//					unsigned char* sendData,
//					size_t sizeofSendData,
//					uint32_t* sendId
//					) = 0;
//		virtual bool cancel(uint32_t sendId) = 0;
//		virtual void disconnectServer() = 0;
//
//		// copyConstructor
//		//AsyncCommClient(const AsyncCommClient& rhs);
//		// substitution operator
//		//AsyncCommClient& operator=(const AsyncCommClient& rhs);
//};
//
//#endif /* ASYNCCOMMCLIENT_HPP_ */
