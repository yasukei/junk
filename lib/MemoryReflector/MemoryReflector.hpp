//#ifndef REFLECTOR_HPP_
//#define REFLECTOR_HPP_

//#include <cstdint>
#include <cstddef>
#include <vector>
#include <map>
#include <pthread.h>

// ------------------------------------------------------------------
typedef pthread_t ReflectionContext;

// ------------------------------------------------------------------
class ReflectionAttribute
{
	public:
		ReflectionAttribute(void);
		ReflectionAttribute(
			void* srcAddr,
			void* dstAddr,
			ReflectionContext srcContext,
			ReflectionContext dstContext,
			size_t sizeofBytes
			);
		~ReflectionAttribute();
		void* srcAddr;
		void* dstAddr;
		ReflectionContext srcContext;
		ReflectionContext dstContext;
		size_t sizeofBytes;
};

// ------------------------------------------------------------------
class ReflectionBuffer
{
	public:
		ReflectionBuffer(const ReflectionAttribute& attr);
		~ReflectionBuffer(void);
		void* getSrcBuffer(void);
		void* getDstBuffer(void);
		void switchSrcBuffer(void);
		void switchDstBuffer(void);

	protected:
		pthread_mutex_t mutex_;
		void* srcBuffer_;
		void* intermediateBuffer_;
		void* dstBuffer_;
		bool intermediateBufferUpdated_;
};

// ------------------------------------------------------------------
class Reflection
{
	public:
		Reflection(const ReflectionAttribute& attr);
		~Reflection(void);
		void reflectSrc(void);
		void reflectDst(void);

	protected:
		ReflectionBuffer buffer_;
		ReflectionAttribute attr_;
};

// ------------------------------------------------------------------
class MemoryReflector
{
	public:
		static MemoryReflector* create(void);
		static void destroy(MemoryReflector*);
		bool enregister(const ReflectionAttribute& attr);
		void reflectSrc(ReflectionContext context);
		void reflectDst(ReflectionContext context);

	protected:
		std::map<ReflectionContext, Reflection*> reflectionsMapBySrcContext_;
		std::map<ReflectionContext, Reflection*> reflectionsMapByDstContext_;
		//~MemoryReflector() {};

		//MemoryReflector(const MemoryReflector& rhs);
		//MemoryReflector& operator=(const MemoryReflector& rhs);
};

//#endif /* REFLECTOR_HPP_ */
