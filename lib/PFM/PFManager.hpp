#ifndef PFMANAGER_HPP_
#define PFMANAGER_HPP_

#include <string>
#include <cstdint>

#define CP do { std::cerr << __FILE__ << "@" << __LINE__ << std::endl; } while (false);

#endif /* PFMANAGER_HPP_ */

//class PFManager
//{
//	public:
//		PFManager() {};
//		~PFManager() {};
//
//		//virtual bool send(
//		//			ACCCallbacks* callbacks,
//		//			unsigned char* sendData,
//		//			size_t sizeofSendData,
//		//			uint32_t* sendId
//		//			) = 0;
//		//virtual bool cancel(uint32_t sendId) = 0;
//		//virtual void disconnectServer() = 0;
//
//		// copyConstructor
//		//AsyncCommClient(const AsyncCommClient& rhs);
//		// substitution operator
//		//AsyncCommClient& operator=(const AsyncCommClient& rhs);
//};


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
//
