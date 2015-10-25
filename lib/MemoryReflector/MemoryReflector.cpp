
#include "MemoryReflector.hpp"
#include <cstddef>
#include <cstring>

using std::vector;
using std::map;
using std::pair;

//#include "private.h"

// ------------------------------------------------------------------
// define
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Helper function
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// ReflectionAttribute
// ------------------------------------------------------------------
ReflectionAttribute::ReflectionAttribute(
	void
	) :
	srcAddr(nullptr),
	dstAddr(nullptr),
	srcContext(0),
	dstContext(0),
	sizeofBytes(0)
{
}

// ------------------------------------------------------------------
ReflectionAttribute::ReflectionAttribute(
	void* srcAddr,
	void* dstAddr,
	ReflectionContext srcContext,
	ReflectionContext dstContext,
	size_t sizeofBytes
	) :
	srcAddr(srcAddr),
	dstAddr(dstAddr),
	srcContext(srcContext),
	dstContext(dstContext),
	sizeofBytes(sizeofBytes)
{
}

// ------------------------------------------------------------------
ReflectionAttribute::~ReflectionAttribute(
	void
	)
{
}

// ------------------------------------------------------------------
// ReflectionBuffer
// ------------------------------------------------------------------
ReflectionBuffer::ReflectionBuffer(
	const ReflectionAttribute& attr
	) :
	mutex_(PTHREAD_MUTEX_INITIALIZER),
	srcBuffer_(malloc(attr.sizeofBytes)),
	intermediateBuffer_(malloc(attr.sizeofBytes)),
	dstBuffer_(malloc(attr.sizeofBytes)),
	intermediateBufferUpdated_(false)
{
}

// ------------------------------------------------------------------
ReflectionBuffer::~ReflectionBuffer(
	void
	)
{
	free(dstBuffer_);
	free(intermediateBuffer_);
	free(srcBuffer_);
	pthread_mutex_destroy(&mutex_);
}

// ------------------------------------------------------------------
void* ReflectionBuffer::getSrcBuffer(
	void
	)
{
	return srcBuffer_;
}

// ------------------------------------------------------------------
void* ReflectionBuffer::getDstBuffer(
	void
	)
{
	return dstBuffer_;
}

// ------------------------------------------------------------------
void ReflectionBuffer::switchSrcBuffer(
	void
	)
{
	void* temp;

	pthread_mutex_lock(&mutex_);
	temp = intermediateBuffer_;
	intermediateBuffer_ = srcBuffer_;
	srcBuffer_ = temp;
	intermediateBufferUpdated_ = true;
	pthread_mutex_unlock(&mutex_);
}

// ------------------------------------------------------------------
void ReflectionBuffer::switchDstBuffer(
	void
	)
{
	void* temp;

	pthread_mutex_lock(&mutex_);
	if( intermediateBufferUpdated_ )
	{
		temp = dstBuffer_;
		dstBuffer_ = intermediateBuffer_;
		intermediateBuffer_ = temp;
		intermediateBufferUpdated_ = false;
	}
	pthread_mutex_unlock(&mutex_);
}

// ------------------------------------------------------------------
// Reflection
// ------------------------------------------------------------------
Reflection::Reflection(
	const ReflectionAttribute& attr
	) :
	buffer_(attr),
	attr_(attr)
{
}

// ------------------------------------------------------------------
Reflection::~Reflection(
	void
	)
{
}

// ------------------------------------------------------------------
void Reflection::reflectSrc(
	void
	)
{
	memcpy(buffer_.getSrcBuffer(), attr_.srcAddr, attr_.sizeofBytes);
	buffer_.switchSrcBuffer();
}

// ------------------------------------------------------------------
void Reflection::reflectDst(
	void
	)
{
	buffer_.switchDstBuffer();
	memcpy(attr_.dstAddr, buffer_.getDstBuffer(), attr_.sizeofBytes);
}

// ------------------------------------------------------------------
// MemoryReflector
// ------------------------------------------------------------------
MemoryReflector* MemoryReflector::create(
	void
	)
{
	return new MemoryReflector();
}

// ------------------------------------------------------------------
void MemoryReflector::destroy(
	MemoryReflector* self
	)
{
	if( (self != nullptr) || (self != NULL) )
	{
		delete(self);
	}
}

// ------------------------------------------------------------------
bool MemoryReflector::enregister(
	const ReflectionAttribute& attr
	)
{
	Reflection* reflection;

	if( (attr.srcAddr == nullptr) ||
		(attr.dstAddr == nullptr) ||
		(attr.sizeofBytes == 0) )
	{
		return false;
	}

	reflection = new Reflection(attr);
	if( reflection == nullptr )
	{
		// TODO: exception
		return false;
	}
	reflectionsMapBySrcContext_.insert(
		pair<ReflectionContext, Reflection* >(attr.srcContext, reflection)
		);
	reflectionsMapByDstContext_.insert(
		pair<ReflectionContext, Reflection* >(attr.dstContext, reflection)
		);
	return true;
}

// ------------------------------------------------------------------
void MemoryReflector::reflectSrc(
	ReflectionContext context
	)
{
	map<ReflectionContext, Reflection*>::iterator iter;
	Reflection* reflection;

	for( iter = reflectionsMapBySrcContext_.find(context);
		( (iter != reflectionsMapBySrcContext_.end()) && (iter->first == context) );
		iter++ )
	{
		reflection = iter->second;
		reflection->reflectSrc();
	}
}

// ------------------------------------------------------------------
void MemoryReflector::reflectDst(
	ReflectionContext context
	)
{
	map<ReflectionContext, Reflection*>::iterator iter;
	Reflection* reflection;

	for( iter = reflectionsMapByDstContext_.find(context);
		( (iter != reflectionsMapByDstContext_.end()) && (iter->first == context) );
		iter++ )
	{
		reflection = iter->second;
		reflection->reflectDst();
	}
}

// ------------------------------------------------------------------
void MemoryReflector::__debugprint(
	void
	)
{
	fprintf(stdout, "reflectionsMapBySrcContext_.size(): [%d]\n",
		reflectionsMapBySrcContext_.size());
	fprintf(stdout, "reflectionsMapByDstContext_.size(): [%d]\n",
		reflectionsMapByDstContext_.size());
}

