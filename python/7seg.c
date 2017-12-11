#include <stdio.h>
#include <stdint.h>

#include "./externals.c"

// TODO: delete
#define Bool	uint8_t
#define TRUE	((Bool)1)
#define FALSE	((Bool)0)

//-------------------------------------------------------------------
// ServiceSwitchDriverWrapper
//-------------------------------------------------------------------
typedef struct ServiceSwitchDriverWrapper ServiceSwitchDriverWrapper;
struct ServiceSwitchDriverWrapper
{
	void (*registerOn)(uint32_t start, uint32_t end, void (*callback)(void* context), void* context);
	void (*registerOff)(uint32_t start, uint32_t end, void (*callback)(void* context), void* context);
};

//-------------------------------------------------------------------
void ServiceSwitchDriverWrapper_initialize(
	ServiceSwitchDriverWrapper* self
	// TODO
	)
{
	// TODO
}

//-------------------------------------------------------------------
void ServiceSwitchDriverWrapper_registerOn(
	ServiceSwitchDriverWrapper* self,
	uint32_t start,
	uint32_t end,
	void (*callback)(void* context),
	void* context
	)
{
	// TODO
}

//-------------------------------------------------------------------
void ServiceSwitchDriverWrapper_registerOff(
	ServiceSwitchDriverWrapper* self,
	uint32_t start,
	uint32_t end,
	void (*callback)(void* context),
	void* context
	)
{
	// TODO
}

//-------------------------------------------------------------------
// SsdDriverWrapper
//-------------------------------------------------------------------
typedef struct SsdDriverWrapper SsdDriverWrapper;
struct SsdDriverWrapper
{
	void (*display)(uint8_t left, uint8_t right);
};

//-------------------------------------------------------------------
void SsdDriverWrapper_initialize(
	SsdDriverWrapper* self,
	void (*display)(uint8_t left, uint8_t right)
	)
{
	self->display = display;
}

//-------------------------------------------------------------------
void SsdDriverWrapper_display(
	SsdDriverWrapper* self,
	uint8_t left,
	uint8_t right
	)
{
	self->display(left, right);
}

//-------------------------------------------------------------------
void SsdDriverWrapperDefaultMothod_display(
	uint8_t left,
	uint8_t right
	)
{
	T_SEVENSEGDRIVER_PATTERN_LIST patternList;

	patternList.pattern = SEVENSEGDRIVER_DISPLAY_CODE(left, right);
	patternList.period = 1000;	// TODO: Is this value right? Does it work?
	patternList.nextList = 0;	// TODO: use NULL
	
	SevenSegmentLEDDriver_setNormalList(&patternList, 1);
}


//-------------------------------------------------------------------
// EventMessanger
//-------------------------------------------------------------------
typedef struct EventMessanger EventMessanger;

#define cMaxNumofSubscribers	4

typedef void (*EventMessangerCallback)(void* context);
struct EventMessanger
{
	int numofSubscribers;
	EventMessangerCallback callbacks[cMaxNumofSubscribers];
	void* contexts[cMaxNumofSubscribers];
};

//-------------------------------------------------------------------
void EventMessanger_initialize(
	EventMessanger* self
	)
{
	self->numofSubscribers = 0;
}

//-------------------------------------------------------------------
void EventMessanger_notifyUpdated(
	EventMessanger* self
	)
{
	int index;

	for(index = 0; index < self->numofSubscribers; index++)
	{
		EventMessangerCallback callback	= self->callbacks[index];
		void* context					= self->contexts[index];

		callback(context);
	}
}

//-------------------------------------------------------------------
Bool EventMessanger_subscribe(
	EventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	if(self->numofSubscribers == cMaxNumofSubscribers)
	{
		return FALSE;
	}

	self->callbacks[self->numofSubscribers] = callback;
	self->contexts[self->numofSubscribers] = context;
	self->numofSubscribers += 1;
	return TRUE;
}

//-------------------------------------------------------------------
// StateEventMessanger
//-------------------------------------------------------------------
typedef struct StateEventMessanger StateEventMessanger;

struct StateEventMessanger
{
	EventMessanger eventMessanger;
	int state;
};

//-------------------------------------------------------------------
void StateEventMessanger_initialize(
	StateEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	self->state = 0;	// TODO: set some default state
}

//-------------------------------------------------------------------
static void __StateEventMessanger_lock(
	StateEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __StateEventMessanger_unlock(
	StateEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
void StateEventMessanger_notifyUpdated(
	StateEventMessanger* self,
	int state
	)
{
	__StateEventMessanger_lock(self);
	self->state = state;
	EventMessanger_notifyUpdated(&self->eventMessanger);
	__StateEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
Bool StateEventMessanger_subscribe(
	StateEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	Bool result;

	__StateEventMessanger_lock(self);
	result = EventMessanger_subscribe(&self->eventMessanger, callback, context);
	__StateEventMessanger_unlock(self);

	return result;
}

//-------------------------------------------------------------------
int StateEventMessanger_getState(
	StateEventMessanger* self
	)
{
	int result;

	__StateEventMessanger_lock(self);
	result = self->state;
	__StateEventMessanger_unlock(self);

	return result;
}

//-------------------------------------------------------------------
// ConfigEventMessangerSignature
//-------------------------------------------------------------------
typedef struct ConfigEventMessangerSignature ConfigEventMessangerSignature;
struct ConfigEventMessangerSignature
{
	Bool	valid;
	uint8_t signatureHigh;
	uint8_t signatureLow;
	uint8_t yearHigh;
	uint8_t yearLow;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
};

//-------------------------------------------------------------------
void ConfigEventMessangerSignature_set(
	ConfigEventMessangerSignature* self,
	Bool	valid,
	uint8_t signatureHigh,
	uint8_t signatureLow,
	uint8_t yearHigh,
	uint8_t yearLow,
	uint8_t month,
	uint8_t day,
	uint8_t hour,
	uint8_t min,
	uint8_t sec
	)
{
	self->valid			= valid;
	self->signatureHigh = signatureHigh;
	self->signatureLow  = signatureLow;
	self->yearHigh      = yearHigh;
	self->yearLow       = yearLow;
	self->month         = month;
	self->day           = day;
	self->hour          = hour;
	self->min           = min;
	self->sec           = sec;
}

//-------------------------------------------------------------------
// ConfigEventMessanger
//-------------------------------------------------------------------
typedef struct ConfigEventMessanger ConfigEventMessanger;
typedef enum e_SsdDisplayCommandForRestore e_SsdDisplayCommandForRestore;

enum e_SsdDisplayCommandForRestore
{
	e_SsdDisplayCommandForRestore_displayInitializing,
	e_SsdDisplayCommandForRestore_displayProcessing,
	e_SsdDisplayCommandForRestore_displaySignature,
	e_SsdDisplayCommandForRestore_displaySignatureWithDot,
	e_SsdDisplayCommandForRestore_displaySignatureWithDotAndDate,
};

struct ConfigEventMessanger
{
	EventMessanger eventMessanger;
	Bool hasSignature;
	ConfigEventMessangerSignature signature;
	e_SsdDisplayCommandForRestore displayCommandForRestore;
};

//-------------------------------------------------------------------
void ConfigEventMessanger_initialize(
	ConfigEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	self->hasSignature = FALSE;
	self->displayCommandForRestore = e_SsdDisplayCommandForRestore_displayInitializing;
}

//-------------------------------------------------------------------
static void __ConfigEventMessanger_lock(
	ConfigEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __ConfigEventMessanger_unlock(
	ConfigEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
void ConfigEventMessanger_notifySignatureUpdated(
	ConfigEventMessanger* self,
	ConfigEventMessangerSignature* signature
	)
{
	__ConfigEventMessanger_lock(self);
	self->signature = *signature;
	EventMessanger_notifyUpdated(&self->eventMessanger);
	__ConfigEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
void ConfigEventMessanger_notifySsdDisplayCommandForRestoreUpdated(
	ConfigEventMessanger* self,
	e_SsdDisplayCommandForRestore displayCommandForRestore
	)
{
	__ConfigEventMessanger_lock(self);
	self->displayCommandForRestore = displayCommandForRestore;
	EventMessanger_notifyUpdated(&self->eventMessanger);
	__ConfigEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
Bool ConfigEventMessanger_subscribe(
	ConfigEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	Bool result;

	__ConfigEventMessanger_lock(self);
	result = EventMessanger_subscribe(&self->eventMessanger, callback, context);
	__ConfigEventMessanger_unlock(self);

	return result;
}

//-------------------------------------------------------------------
void ConfigEventMessanger_getSignature(
	ConfigEventMessanger* self,
	ConfigEventMessangerSignature* signature	// out
	)
{
	__ConfigEventMessanger_lock(self);
	*signature = self->signature;
	__ConfigEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
e_SsdDisplayCommandForRestore ConfigEventMessanger_getDisplayCommandForRestore(
	ConfigEventMessanger* self
	)
{
	e_SsdDisplayCommandForRestore result;

	__ConfigEventMessanger_lock(self);
	result = self->displayCommandForRestore;
	__ConfigEventMessanger_unlock(self);

	return result;
}

//-------------------------------------------------------------------
// DataLoggingEventMessangerDotPattern
//-------------------------------------------------------------------
typedef enum e_DataLoggingEventMessangerDotPattern e_DataLoggingEventMessangerDotPattern;

enum e_DataLoggingEventMessangerDotPattern
{
	e_DataLoggingEventMessangerDotPattern_on,
	e_DataLoggingEventMessangerDotPattern_off,
	e_DataLoggingEventMessangerDotPattern_blink,
};


//-------------------------------------------------------------------
// DataLoggingEventMessanger
//-------------------------------------------------------------------
typedef struct DataLoggingEventMessanger DataLoggingEventMessanger;

struct DataLoggingEventMessanger
{
	EventMessanger eventMessanger;
	e_DataLoggingEventMessangerDotPattern left;
	e_DataLoggingEventMessangerDotPattern right;
};

//-------------------------------------------------------------------
void DataLoggingEventMessanger_initialize(
	DataLoggingEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	self->left = e_DataLoggingEventMessangerDotPattern_off;
	self->right = e_DataLoggingEventMessangerDotPattern_off;
}

//-------------------------------------------------------------------
static void __DataLoggingEventMessanger_lock(
	DataLoggingEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __DataLoggingEventMessanger_unlock(
	DataLoggingEventMessanger* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
void DataLoggingEventMessanger_notifyUpdated(
	DataLoggingEventMessanger* self,
	e_DataLoggingEventMessangerDotPattern left,
	e_DataLoggingEventMessangerDotPattern right
	)
{
	__DataLoggingEventMessanger_lock(self);
	self->left = left;
	self->right = right;
	EventMessanger_notifyUpdated(&self->eventMessanger);
	__DataLoggingEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
Bool DataLoggingEventMessanger_subscribe(
	DataLoggingEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	Bool result;

	__DataLoggingEventMessanger_lock(self);
	result = EventMessanger_subscribe(&self->eventMessanger, callback, context);
	__DataLoggingEventMessanger_unlock(self);

	return result;
}

//-------------------------------------------------------------------
void DataLoggingEventMessanger_getDotPattern(
	DataLoggingEventMessanger* self,
	e_DataLoggingEventMessangerDotPattern* left,	// out
	e_DataLoggingEventMessangerDotPattern* right	// out
	)
{
	__DataLoggingEventMessanger_lock(self);
	*left = self->left;
	*right = self->right;
	__DataLoggingEventMessanger_unlock(self);
}

//-------------------------------------------------------------------
// FaultLogWatcher
//-------------------------------------------------------------------
typedef struct FaultLogWatcher FaultLogWatcher;

struct FaultLogWatcher
{
	EventMessanger eventMessanger;
};

//-------------------------------------------------------------------
void FaultLogWatcher_initialize(
	FaultLogWatcher* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	// TODO: register FaultLogWatcher_doCyclicTask() into ApplicationTimer
}

//-------------------------------------------------------------------
static void __FaultLogWatcher_lock(
	FaultLogWatcher* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __FaultLogWatcher_unlock(
	FaultLogWatcher* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __FaultLogWatcher_notifyUpdated(
	FaultLogWatcher* self
	)
{
	__FaultLogWatcher_lock(self);
	EventMessanger_notifyUpdated(&self->eventMessanger);
	__FaultLogWatcher_unlock(self);
}

//-------------------------------------------------------------------
void FaultLogWatcher_doCyclicTask(
	FaultLogWatcher* self
	)
{
	// TODO: lock & unlock

	/*
	 * tentative code
	if(EventManager_getUpdateCount() != self->previousUpdateCount)
	{
		records = EventManager_readOldestRecordsSatisfyingSpecificCondition();
		self->errorCodes = __translate(records);
		EventMessanger_notifyUpdated(&self->eventMessanger);
	}
	*/
}

//-------------------------------------------------------------------
Bool FaultLogWatcher_subscribe(
	FaultLogWatcher* self,
	EventMessangerCallback callback,
	void* context
	)
{
	Bool result;

	__FaultLogWatcher_lock(self);
	result = EventMessanger_subscribe(&self->eventMessanger, callback, context);
	__FaultLogWatcher_unlock(self);

	return result;
}

//-------------------------------------------------------------------
void FaultLogWatcher_readFaultLog(
	FaultLogWatcher* self
	// TODO: decide arguments
	)
{
	// TODO: lock & unlock
}

//-------------------------------------------------------------------
// SsdPicture
//-------------------------------------------------------------------
typedef uint8_t SsdPicture;

//-------------------------------------------------------------------
// SsdFrame
//-------------------------------------------------------------------
typedef struct SsdFrameMethods SsdFrameMethods;
typedef struct SsdFrame SsdFrame;

typedef SsdPicture	(*SsdFrameMethod_getPicture)(SsdFrame* self);
typedef uint32_t	(*SsdFrameMethod_getDurationTime)(SsdFrame* self);

struct SsdFrameMethods
{
	SsdFrameMethod_getPicture		getPicture;
	SsdFrameMethod_getDurationTime	getDurationTime;
};

struct SsdFrame
{
	SsdFrameMethods* methods;

	SsdFrame* next;	// for SsdAnimation
};

//-------------------------------------------------------------------
void SsdFrame_initializeMethods(
	SsdFrame* self,
	SsdFrameMethods* methods
	)
{
	self->methods = methods;
}

//-------------------------------------------------------------------
SsdPicture SsdFrame_getPicture(
	SsdFrame* self
	)
{
	return self->methods->getPicture(self);
}

//-------------------------------------------------------------------
uint32_t SsdFrame_getDurationTime(
	SsdFrame* self
	)
{
	return self->methods->getDurationTime(self);
}

//-------------------------------------------------------------------
// SsdStaticFrame
//-------------------------------------------------------------------
typedef struct SsdStaticFrame SsdStaticFrame;
struct SsdStaticFrame
{
	SsdFrameMethods* methods;

	SsdPicture picture;
	uint32_t durationTime;
};

//-------------------------------------------------------------------
SsdPicture SsdStaticFrame_getPicture(
	SsdFrame* base
	)
{
	SsdStaticFrame* self = (SsdStaticFrame*)base;

	return self->picture;
}

//-------------------------------------------------------------------
uint32_t SsdStaticFrame_getDurationTime(
	SsdFrame* base
	)
{
	SsdStaticFrame* self = (SsdStaticFrame*)base;

	return self->durationTime;
}

//-------------------------------------------------------------------
SsdFrameMethods G_SsdStaticFrameMethods =
{
	SsdStaticFrame_getPicture,
	SsdStaticFrame_getDurationTime,
};

//-------------------------------------------------------------------
void SsdStaticFrame_initialize(
	SsdStaticFrame* self,
	SsdPicture picture,
	uint32_t durationTime
	)
{
	SsdFrame_initializeMethods(
		(SsdFrame*)self,
		&G_SsdStaticFrameMethods
		);

	self->picture = picture;
	self->durationTime = durationTime;
}

//-------------------------------------------------------------------
// SsdDynamicFrame
//-------------------------------------------------------------------
typedef struct SsdDynamicFrame SsdDynamicFrame;

struct SsdDynamicFrame
{
	SsdFrameMethods* methods;

	SsdPicture (*getPicture)(void* context);
	void* context;
	uint32_t durationTime;
};

//-------------------------------------------------------------------
SsdPicture SsdDynamicFrame_getPicture(
	SsdFrame* base
	)
{
	SsdDynamicFrame* self = (SsdDynamicFrame*)base;

	return self->getPicture(self->context);
}

//-------------------------------------------------------------------
uint32_t SsdDynamicFrame_getDurationTime(
	SsdFrame* base
	)
{
	SsdDynamicFrame* self = (SsdDynamicFrame*)base;

	return self->durationTime;
}

//-------------------------------------------------------------------
SsdFrameMethods G_SsdDynamicFrameMethods =
{
	SsdDynamicFrame_getPicture,
	SsdDynamicFrame_getDurationTime,
};

//-------------------------------------------------------------------
void SsdDynamicFrame_initialize(
	SsdDynamicFrame* self,
	SsdPicture (*getPicture)(void* context),
	void* context,
	uint32_t durationTime
	)
{
	SsdFrame_initializeMethods(
		(SsdFrame*)self,
		&G_SsdDynamicFrameMethods
		);

	self->getPicture = getPicture;
	self->context = context;
	self->durationTime = durationTime;
}

//-------------------------------------------------------------------
// SsdAnimation
//-------------------------------------------------------------------
typedef struct SsdAnimation SsdAnimation;

struct SsdAnimation
{
	SsdFrame* headFrame;
	SsdFrame* tailFrame;
	SsdFrame* currentFrame;
	uint32_t elapsedTimeMillisecOfCurrentFrame;
};

//-------------------------------------------------------------------
void SsdAnimation_initialize(
	SsdAnimation* self
	)
{
	self->headFrame = NULL;
	self->tailFrame = NULL;
	self->currentFrame = NULL;
	self->elapsedTimeMillisecOfCurrentFrame = 0;
}

//-------------------------------------------------------------------
void SsdAnimation_addFrame(
	SsdAnimation* self,
	SsdFrame* frame
	)
{
	if(self->headFrame == NULL)
	{
		self->headFrame = frame;
		self->tailFrame = frame;
	}
	else
	{
		self->tailFrame->next = frame;
		self->tailFrame = frame;
	}
}

//-------------------------------------------------------------------
void SsdAnimation_restartAnimation(
	SsdAnimation* self
	)
{
	self->currentFrame = self->headFrame;
	self->elapsedTimeMillisecOfCurrentFrame = 0;
}

//-------------------------------------------------------------------
SsdPicture SsdAnimation_getFirstPicture(
	SsdAnimation* self
	)
{
	return SsdFrame_getPicture(self->headFrame);
}

//-------------------------------------------------------------------
static SsdPicture __SsdAnimation_getNextPicture(
	SsdAnimation* self,
	Bool* isFinished	// out
	)
{
	SsdFrame* nextFrame;
	SsdPicture lastFramePicture;

	nextFrame = self->currentFrame->next;
	if(nextFrame != NULL)
	{
		self->currentFrame = nextFrame;
		self->elapsedTimeMillisecOfCurrentFrame = 0;
		return SsdFrame_getPicture(nextFrame);
	}

	*isFinished = TRUE;
	lastFramePicture = SsdFrame_getPicture(self->currentFrame);
	SsdAnimation_restartAnimation(self);
	return lastFramePicture;
}

//-------------------------------------------------------------------
SsdPicture SsdAnimation_getPicture(
	SsdAnimation* self,
	uint32_t elapsedTimeMillisec,
	Bool* isFinished	// out
	)
{
	*isFinished = FALSE;
	self->elapsedTimeMillisecOfCurrentFrame += elapsedTimeMillisec;

	if(SsdFrame_getDurationTime(self->currentFrame) > self->elapsedTimeMillisecOfCurrentFrame)
	{
		return SsdFrame_getPicture(self->currentFrame);
	}

	return __SsdAnimation_getNextPicture(self, isFinished);
}

//-------------------------------------------------------------------
// SsdModeState
//-------------------------------------------------------------------
typedef struct SsdModeStateMethods SsdModeStateMethods;
typedef struct SsdModeState SsdModeState;

typedef void (*SsdModeStateMethod_onEntry)(SsdModeState* self);
typedef void (*SsdModeStateMethod_onExit)(SsdModeState* self);
typedef void (*SsdModeStateMethod_onTick)(SsdModeState* self, uint32_t elapsedTimeMillisec);

struct SsdModeStateMethods
{
	SsdModeStateMethod_onEntry	onEntry;
	SsdModeStateMethod_onExit	onExit;
	SsdModeStateMethod_onTick	onTick;
};

struct SsdModeState
{
	SsdModeStateMethods* methods;
};

//-------------------------------------------------------------------
void SsdModeState_initializeMethods(
	SsdModeState* self,
	SsdModeStateMethods* methods
	)
{
	self->methods = methods;
}

//-------------------------------------------------------------------
void SsdModeState_doNothing(
	SsdModeState* self
	)
{
	(void)self;
}

//-------------------------------------------------------------------
void SsdModeState_onEntry(
	SsdModeState* self
	)
{
	self->methods->onEntry(self);
}

//-------------------------------------------------------------------
void SsdModeState_onExit(
	SsdModeState* self
	)
{
	self->methods->onExit(self);
}

//-------------------------------------------------------------------
void SsdModeState_onTick(
	SsdModeState* self,
	uint32_t elapsedTimeMillisec
	)
{
	self->methods->onTick(self, elapsedTimeMillisec);
}

//-------------------------------------------------------------------
// SsdTestMode
//-------------------------------------------------------------------
typedef struct SsdTestMode SsdTestMode;

#define cSsdTestMode_numofFrames	9

struct SsdTestMode
{
	SsdModeState base;
	SsdStaticFrame frames[cSsdTestMode_numofFrames];
	SsdAnimation animation;
	SsdDriverWrapper* ssdDriver;
};

//-------------------------------------------------------------------
void SsdTestMode_onEntry(
	SsdModeState* base
	)
{
	SsdTestMode* self = (SsdTestMode*)base;
	SsdPicture picture;

	SsdAnimation_restartAnimation(&self->animation);
	picture = SsdAnimation_getFirstPicture(&self->animation);
	SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
void SsdTestMode_onTick(
	SsdModeState* base,
	uint32_t elapsedTimeMillisec
	)
{
	SsdTestMode* self = (SsdTestMode*)base;
	SsdPicture picture;
	Bool isFinished = FALSE;

	picture = SsdAnimation_getPicture(&self->animation, elapsedTimeMillisec, &isFinished);
	// TODO: do something when isFinished
	SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
SsdModeStateMethods G_SsdTestModeMethods =
{
	SsdTestMode_onEntry,
	SsdModeState_doNothing,
	SsdTestMode_onTick,
};

//-------------------------------------------------------------------
void SsdTestMode_initialize(
	SsdTestMode* self,
	SsdDriverWrapper* ssdDriver
	)
{
	int index;

	SsdModeState_initializeMethods(
		(SsdModeState*)self,
		&G_SsdTestModeMethods
		);

	SsdStaticFrame_initialize(&self->frames[0], SEVENSEGDRIVER_BITPTN_LEFTUP,		250);
	SsdStaticFrame_initialize(&self->frames[1], SEVENSEGDRIVER_BITPTN_TOP,			250);
	SsdStaticFrame_initialize(&self->frames[2], SEVENSEGDRIVER_BITPTN_RIGHTUP,		250);
	SsdStaticFrame_initialize(&self->frames[3], SEVENSEGDRIVER_BITPTN_RIGHTDOWN,	250);
	SsdStaticFrame_initialize(&self->frames[4], SEVENSEGDRIVER_BITPTN_BOTTOM,		250);
	SsdStaticFrame_initialize(&self->frames[5], SEVENSEGDRIVER_BITPTN_LEFTDOWN,		250);
	SsdStaticFrame_initialize(&self->frames[6], SEVENSEGDRIVER_BITPTN_DASH,			250);
	SsdStaticFrame_initialize(&self->frames[7], SEVENSEGDRIVER_BITPTN_DOT,			250);
	SsdStaticFrame_initialize(&self->frames[8], SEVENSEGDRIVER_BITPTN_BLANK,		250);
	SsdAnimation_initialize(&self->animation);
	for(index = 0; index < cSsdTestMode_numofFrames; index++)
	{
		SsdAnimation_addFrame(&self->animation, (SsdFrame*)&self->frames[index]);
	}

	self->ssdDriver = ssdDriver;
}

//-------------------------------------------------------------------
// SsdNormalMode
//-------------------------------------------------------------------
typedef struct SsdNormalMode SsdNormalMode;

#define cSsdNormalMode_numofFramesForDebug			1	// TODO: convert array to one variable
#define cSsdNormalMode_numofFramesForProgram		2
#define cSsdNormalMode_numofFramesForDotOn			1	// TODO: convert array to one variable
#define cSsdNormalMode_numofFramesForDotOff			1	// TODO: convert array to one variable
#define cSsdNormalMode_numofFramesForDotBlink		2

struct SsdNormalMode
{
	SsdModeState base;

	// ForRun
	SsdDynamicFrame leftFrameForRun;
	SsdDynamicFrame rightFrameForRun;
	SsdAnimation leftAnimationForRun;
	SsdAnimation rightAnimationForRun;

	// ForDebug
	SsdStaticFrame leftFramesForDebug[cSsdNormalMode_numofFramesForDebug];
	SsdStaticFrame rightFramesForDebug[cSsdNormalMode_numofFramesForDebug];
	SsdAnimation leftAnimationForDebug;
	SsdAnimation rightAnimationForDebug;

	// ForProgram
	SsdStaticFrame leftFramesForProgram[cSsdNormalMode_numofFramesForProgram];
	SsdStaticFrame rightFramesForProgram[cSsdNormalMode_numofFramesForProgram];
	SsdAnimation leftAnimationForProgram;
	SsdAnimation rightAnimationForProgram;

	// ForDot
	SsdStaticFrame dotOnFrames[cSsdNormalMode_numofFramesForDotOn];
	SsdStaticFrame dotOffFrames[cSsdNormalMode_numofFramesForDotOff];
	SsdStaticFrame leftDotBlinkFrames[cSsdNormalMode_numofFramesForDotBlink];
	SsdStaticFrame rightDotBlinkFrames[cSsdNormalMode_numofFramesForDotBlink];
	SsdAnimation dotOnAnimation;;
	SsdAnimation dotOffAnimation;;
	SsdAnimation leftDotBlinkAnimation;;
	SsdAnimation rightDotBlinkAnimation;;

	// For onTick
	SsdAnimation* leftAnimation;
	SsdAnimation* rightAnimation;
	SsdAnimation* leftDotAnimation;
	SsdAnimation* rightDotAnimation;

	SsdDriverWrapper* ssdDriver;
	StateEventMessanger* stateEventMessanger;
	ConfigEventMessanger* configEventMessanger;
	DataLoggingEventMessanger* dataLoggingEventMessanger;
};

//-------------------------------------------------------------------
static void __SsdNormalMode_lock(
	SsdNormalMode* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __SsdNormalMode_unlock(
	SsdNormalMode* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
void SsdNormalMode_onEntry(
	SsdModeState* base
	)
{
	SsdNormalMode* self = (SsdNormalMode*)base;
	SsdPicture picture;

	//SsdAnimation_restartAnimation(&self->animation);
	//picture = SsdAnimation_getFirstPicture(&self->animation);
	//SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
void SsdNormalMode_onTick(
	SsdModeState* base,
	uint32_t elapsedTimeMillisec
	)
{
	SsdNormalMode* self = (SsdNormalMode*)base;
	SsdPicture left;
	SsdPicture right;
	SsdPicture leftDot;
	SsdPicture rightDot;
	Bool dummy;

	__SsdNormalMode_lock(self);
	left		= SsdAnimation_getPicture(self->leftAnimation, elapsedTimeMillisec, &dummy);
	right		= SsdAnimation_getPicture(self->rightAnimation, elapsedTimeMillisec, &dummy);
	leftDot		= SsdAnimation_getPicture(self->leftDotAnimation, elapsedTimeMillisec, &dummy);
	rightDot	= SsdAnimation_getPicture(self->rightDotAnimation, elapsedTimeMillisec, &dummy);
	__SsdNormalMode_unlock(self);

	SsdDriverWrapper_display(self->ssdDriver, left | leftDot, right | rightDot);
}

//-------------------------------------------------------------------
SsdModeStateMethods G_SsdNormalModeMethods =
{
	SsdNormalMode_onEntry,
	SsdModeState_doNothing,
	SsdNormalMode_onTick,
};

//-------------------------------------------------------------------
// TODO: replace
typedef enum e_State e_State;
enum e_State
{
	e_State_Run,
	e_State_Debug,
};

//-------------------------------------------------------------------
static void __SsdNormalMode_changeAnimation(
	SsdNormalMode* self,
	SsdAnimation* leftAnimation,
	SsdAnimation* rightAnimation
	)
{
	__SsdNormalMode_lock(self);
	self->leftAnimation = leftAnimation;
	self->rightAnimation = rightAnimation;
	SsdAnimation_restartAnimation(self->leftAnimation);
	SsdAnimation_restartAnimation(self->rightAnimation);
	__SsdNormalMode_unlock(self);
}

//-------------------------------------------------------------------
void __SsdNormalMode_changeDotAnimation(
	SsdNormalMode* self,
	SsdAnimation* leftDotAnimation,
	SsdAnimation* rightDotAnimation
	)
{
	__SsdNormalMode_lock(self);
	self->leftDotAnimation = leftDotAnimation;
	self->rightDotAnimation = rightDotAnimation;
	SsdAnimation_restartAnimation(self->leftDotAnimation);
	SsdAnimation_restartAnimation(self->rightDotAnimation);
	__SsdNormalMode_unlock(self);
}

//-------------------------------------------------------------------
void __SsdNormalMode_notifyStateManagerUpdated(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	int state;

	state = StateEventMessanger_getState(self->stateEventMessanger);
	switch(state)
	{
		case e_State_Run:
			__SsdNormalMode_changeAnimation(
				self,
				&self->leftAnimationForRun,
				&self->rightAnimationForRun
				);
			break;
		case e_State_Debug:
			__SsdNormalMode_changeAnimation(
				self,
				&self->leftAnimationForDebug,
				&self->rightAnimationForDebug
				);
			break;
		default:
			__SsdNormalMode_changeAnimation(
				self,
				&self->leftAnimationForProgram,
				&self->rightAnimationForProgram
				);
			break;
	}
}

//-------------------------------------------------------------------
void __SsdNormalMode_notifyConfigManagerUpdated(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;

	// do nothing
}

//-------------------------------------------------------------------
SsdAnimation* __SsdNormalMode_chooseAnimationByDotPattern(
	SsdNormalMode* self,
	e_DataLoggingEventMessangerDotPattern dotPattern,
	SsdAnimation* onAnimation,
	SsdAnimation* offAnimation,
	SsdAnimation* blinkAnimation
	)
{
	SsdAnimation* result;
	(void)self;

	switch(dotPattern)
	{
		case e_DataLoggingEventMessangerDotPattern_on:
			result = onAnimation;
			break;
		case e_DataLoggingEventMessangerDotPattern_blink:
			result = blinkAnimation;
			break;
		case e_DataLoggingEventMessangerDotPattern_off:
			// fall through
		default:
			result = offAnimation;
			break;
	}
	return result;
}

//-------------------------------------------------------------------
void __SsdNormalMode_notifyDataLoggingManagerUpdated(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	e_DataLoggingEventMessangerDotPattern leftDotPattern;
	e_DataLoggingEventMessangerDotPattern rightDotPattern;
	SsdAnimation* leftDotAnimation;
	SsdAnimation* rightDotAnimation;

	DataLoggingEventMessanger_getDotPattern(
		self->dataLoggingEventMessanger,
		&leftDotPattern,
		&rightDotPattern
		);

	leftDotAnimation = __SsdNormalMode_chooseAnimationByDotPattern(
									self,
									leftDotPattern,
									&self->dotOnAnimation,
									&self->dotOffAnimation,
									&self->leftDotBlinkAnimation
									);
	rightDotAnimation = __SsdNormalMode_chooseAnimationByDotPattern(
									self,
									rightDotPattern,
									&self->dotOnAnimation,
									&self->dotOffAnimation,
									&self->rightDotBlinkAnimation
									);
	__SsdNormalMode_changeDotAnimation(self, leftDotAnimation, rightDotAnimation);
}

//-------------------------------------------------------------------
#define m_GetUpper4bit(byte)	(((byte) >> 4) & 0x0F)
#define m_GetLower4bit(byte)	(((byte) >> 0) & 0x0F)

//-------------------------------------------------------------------
SsdPicture __SsdNormalMode_getLeftPictureOfSignature(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(! signature.valid)
	{
		return SEVENSEGDRIVER_BITPTN_DASH;
	}
	return SEVENSEGDRIVER_DISPLAY_HEX(m_GetUpper4bit(signature.signatureLow));
}

//-------------------------------------------------------------------
SsdPicture __SsdNormalMode_getRightPictureOfSignature(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(! signature.valid)
	{
		return SEVENSEGDRIVER_BITPTN_DASH;
	}
	return SEVENSEGDRIVER_DISPLAY_HEX(m_GetLower4bit(signature.signatureLow));
}

//-------------------------------------------------------------------
void __SsdNormalMode_initializeAnimationForRun(
	SsdNormalMode* self
	)
{
	SsdDynamicFrame_initialize(
		&self->leftFrameForRun,
		__SsdNormalMode_getLeftPictureOfSignature,
		(void*)self,
		0
		);
	SsdDynamicFrame_initialize(
		&self->rightFrameForRun,
		__SsdNormalMode_getRightPictureOfSignature,
		(void*)self,
		0
		);
	SsdAnimation_initialize(&self->leftAnimationForRun);
	SsdAnimation_initialize(&self->rightAnimationForRun);
	SsdAnimation_addFrame(&self->leftAnimationForRun,		(SsdFrame*)&self->leftFrameForRun);
	SsdAnimation_addFrame(&self->rightAnimationForRun,		(SsdFrame*)&self->rightFrameForRun);
}

//-------------------------------------------------------------------
void __SsdNormalMode_initializeAnimationForDebug(
	SsdNormalMode* self
	)
{
	int index;

	SsdStaticFrame_initialize(&self->leftFramesForDebug[0],		SEVENSEGDRIVER_BITPTN_DASH,	0);
	SsdStaticFrame_initialize(&self->rightFramesForDebug[0],	SEVENSEGDRIVER_BITPTN_DASH,	0);
	SsdAnimation_initialize(&self->leftAnimationForDebug);
	SsdAnimation_initialize(&self->rightAnimationForDebug);
	for(index = 0; index < cSsdNormalMode_numofFramesForDebug; index++)
	{
		SsdAnimation_addFrame(&self->leftAnimationForDebug,		(SsdFrame*)&self->leftFramesForDebug[index]);
		SsdAnimation_addFrame(&self->rightAnimationForDebug,	(SsdFrame*)&self->rightFramesForDebug[index]);
	}

}

//-------------------------------------------------------------------
void __SsdNormalMode_initializeAnimationForProgram(
	SsdNormalMode* self
	)
{
	int index;

	SsdStaticFrame_initialize(&self->leftFramesForProgram[0],	SEVENSEGDRIVER_BITPTN_DASH,		500);
	SsdStaticFrame_initialize(&self->leftFramesForProgram[1],	SEVENSEGDRIVER_BITPTN_BLANK,	500);
	SsdStaticFrame_initialize(&self->rightFramesForProgram[0],	SEVENSEGDRIVER_BITPTN_DASH,		500);
	SsdStaticFrame_initialize(&self->rightFramesForProgram[1],	SEVENSEGDRIVER_BITPTN_BLANK,	500);
	SsdAnimation_initialize(&self->leftAnimationForProgram);
	SsdAnimation_initialize(&self->rightAnimationForProgram);
	for(index = 0; index < cSsdNormalMode_numofFramesForProgram; index++)
	{
		SsdAnimation_addFrame(&self->leftAnimationForProgram,	(SsdFrame*)&self->leftFramesForProgram[index]);
		SsdAnimation_addFrame(&self->rightAnimationForProgram,	(SsdFrame*)&self->rightFramesForProgram[index]);
	}
}

//-------------------------------------------------------------------
void __SsdNormalMode_initializeAnimationForDot(
	SsdNormalMode* self
	)
{
	int index;

	SsdStaticFrame_initialize(&self->dotOnFrames[0],	SEVENSEGDRIVER_BITPTN_DOT,		0);
	SsdAnimation_initialize(&self->dotOnAnimation);
	for(index = 0; index < cSsdNormalMode_numofFramesForDotOn; index++)
	{
		SsdAnimation_addFrame(&self->dotOnAnimation,	(SsdFrame*)&self->dotOnFrames[index]);
	}

	SsdStaticFrame_initialize(&self->dotOffFrames[0],	SEVENSEGDRIVER_BITPTN_BLANK,	0);
	SsdAnimation_initialize(&self->dotOffAnimation);
	for(index = 0; index < cSsdNormalMode_numofFramesForDotOff; index++)
	{
		SsdAnimation_addFrame(&self->dotOffAnimation,	(SsdFrame*)&self->dotOffFrames[index]);
	}

	SsdStaticFrame_initialize(&self->leftDotBlinkFrames[0],		SEVENSEGDRIVER_BITPTN_DOT,		500);
	SsdStaticFrame_initialize(&self->leftDotBlinkFrames[1],		SEVENSEGDRIVER_BITPTN_BLANK,	500);
	SsdStaticFrame_initialize(&self->rightDotBlinkFrames[0],	SEVENSEGDRIVER_BITPTN_DOT,		500);
	SsdStaticFrame_initialize(&self->rightDotBlinkFrames[1],	SEVENSEGDRIVER_BITPTN_BLANK,	500);
	SsdAnimation_initialize(&self->leftDotBlinkAnimation);
	SsdAnimation_initialize(&self->rightDotBlinkAnimation);
	for(index = 0; index < cSsdNormalMode_numofFramesForDotBlink; index++)
	{
		SsdAnimation_addFrame(&self->leftDotBlinkAnimation,		(SsdFrame*)&self->leftDotBlinkFrames[index]);
		SsdAnimation_addFrame(&self->rightDotBlinkAnimation,	(SsdFrame*)&self->rightDotBlinkFrames[index]);
	}
}

//-------------------------------------------------------------------
Bool __SsdNormalMode_subscribeAllEventMessangers(
	SsdNormalMode* self,
	StateEventMessanger* stateEventMessanger,
	ConfigEventMessanger* configEventMessanger,
	DataLoggingEventMessanger* dataLoggingEventMessanger
	)
{
	if(! StateEventMessanger_subscribe(
			stateEventMessanger,
			__SsdNormalMode_notifyStateManagerUpdated,
			(void*)self) )
	{
		return FALSE;
	}
	if(! ConfigEventMessanger_subscribe(
			configEventMessanger,
			__SsdNormalMode_notifyConfigManagerUpdated,
			(void*)self) )
	{
		return FALSE;
	}
	if(! DataLoggingEventMessanger_subscribe(
			dataLoggingEventMessanger,
			__SsdNormalMode_notifyDataLoggingManagerUpdated,
			(void*)self) )
	{
		return FALSE;
	}

	self->stateEventMessanger = stateEventMessanger;
	self->configEventMessanger = configEventMessanger;
	self->dataLoggingEventMessanger = dataLoggingEventMessanger;
	return TRUE;
}

//-------------------------------------------------------------------
Bool SsdNormalMode_initialize(
	SsdNormalMode* self,
	SsdDriverWrapper* ssdDriver,
	StateEventMessanger* stateEventMessanger,
	ConfigEventMessanger* configEventMessanger,
	DataLoggingEventMessanger* dataLoggingEventMessanger
	)
{
	SsdModeState_initializeMethods(
		(SsdModeState*)self,
		&G_SsdNormalModeMethods
		);

	__SsdNormalMode_initializeAnimationForRun(self);
	__SsdNormalMode_initializeAnimationForDebug(self);
	__SsdNormalMode_initializeAnimationForProgram(self);
	__SsdNormalMode_initializeAnimationForDot(self);

	self->ssdDriver = ssdDriver;

	return __SsdNormalMode_subscribeAllEventMessangers(
				self,
				stateEventMessanger,
				configEventMessanger,
				dataLoggingEventMessanger
				);
}

//-------------------------------------------------------------------
// SsdRestoreMode
//-------------------------------------------------------------------
typedef struct SsdRestoreMode SsdRestoreMode;

#define cSsdRestoreMode_numofFramesForInitializing				8
#define cSsdRestoreMode_numofFramesForProcessing				4
#define cSsdRestoreMode_numofFramesForSignature					2
#define cSsdRestoreMode_numofFramesForSignatureWithDot			2
#define cSsdRestoreMode_numofFramesForSignatureWithDotAndDate	13

struct SsdRestoreMode
{
	SsdModeState base;

	// ForInitializing
	SsdStaticFrame framesForInitializing[cSsdRestoreMode_numofFramesForInitializing];
	SsdAnimation animationForInitializing;

	// ForProcessing
	SsdStaticFrame leftFramesForProcessing[cSsdRestoreMode_numofFramesForProcessing];
	SsdStaticFrame rightFramesForProcessing[cSsdRestoreMode_numofFramesForProcessing];
	SsdAnimation leftAnimationForProcessing;
	SsdAnimation rightAnimationForProcessing;

	// ForSignature
	SsdDynamicFrame leftFramesForSignature[cSsdRestoreMode_numofFramesForSignature];
	SsdDynamicFrame rightFramesForSignature[cSsdRestoreMode_numofFramesForSignature];
	SsdAnimation leftAnimationForSignature;
	SsdAnimation rightAnimationForSignature;

	// ForSignatureWithDot
	SsdDynamicFrame framesForSignatureWithDot[cSsdRestoreMode_numofFramesForSignatureWithDot];
	SsdAnimation animationForSignatureWithDot;

	// ForSignatureWithDotAndDate
	SsdDynamicFrame framesForSignatureWithDotAndDate[cSsdRestoreMode_numofFramesForSignatureWithDotAndDate];
	SsdAnimation animationForSignatureWithDotAndDate;

	SsdDriverWrapper* ssdDriver;
};

//-------------------------------------------------------------------
void SsdRestoreMode_onEntry(
	SsdModeState* base
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)base;
	SsdPicture picture;

	// TODO
	//SsdAnimation_restartAnimation(&self->animation);
	//picture = SsdAnimation_getFirstPicture(&self->animation);
	//SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
void SsdRestoreMode_onTick(
	SsdModeState* base,
	uint32_t elapsedTimeMillisec
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)base;
	SsdPicture picture;
	Bool isFinished = FALSE;

	// TODO
	//picture = SsdAnimation_getPicture(&self->animation, elapsedTimeMillisec, &isFinished);
	//SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
SsdModeStateMethods G_SsdRestoreModeMethods =
{
	SsdRestoreMode_onEntry,
	SsdModeState_doNothing,
	SsdRestoreMode_onTick,
};

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_getLeftPictureOfSignature(
	SsdRestoreMode* self,
	uint8_t signature1Byte
	)
{
	return SEVENSEGDRIVER_DISPLAY_HEX(m_GetUpper4bit(signature1Byte));
}

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_getRightPictureOfSignature(
	SsdRestoreMode* self,
	uint8_t signature1Byte
	)
{
	return SEVENSEGDRIVER_DISPLAY_HEX(m_GetLower4bit(signature1Byte));
}

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_get1stLeftPictureOfSignature(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	return __SsdRestoreMode_getLeftPictureOfSignature(self, signature.signatureHigh);
}

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_get1stRightPictureOfSignature(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	return __SsdRestoreMode_getRightPictureOfSignature(self, signature.signatureHigh);
}

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_get2ndLeftPictureOfSignature(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	return __SsdRestoreMode_getLeftPictureOfSignature(self, signature.signatureLow);
}

//-------------------------------------------------------------------
SsdPicture __SsdRestoreMode_get2ndRightPictureOfSignature(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	return __SsdRestoreMode_getRightPictureOfSignature(self, signature.signatureLow);
}

//-------------------------------------------------------------------
void SsdRestoreMode_initialize(
	SsdRestoreMode* self,
	SsdDriverWrapper* ssdDriver
	)
{
	int index;

	SsdModeState_initializeMethods(
		(SsdModeState*)self,
		&G_SsdRestoreModeMethods
		);

	// ForInitializing
	SsdStaticFrame_initialize(&self->framesForInitializing[0], SEVENSEGDRIVER_BITPTN_LEFTUP,		250);
	SsdStaticFrame_initialize(&self->framesForInitializing[1], SEVENSEGDRIVER_BITPTN_TOP,			250);
	SsdStaticFrame_initialize(&self->framesForInitializing[2], SEVENSEGDRIVER_BITPTN_RIGHTUP,		250);
	SsdStaticFrame_initialize(&self->framesForInitializing[3], SEVENSEGDRIVER_BITPTN_RIGHTDOWN,		250);
	SsdStaticFrame_initialize(&self->framesForInitializing[4], SEVENSEGDRIVER_BITPTN_BOTTOM,		250);
	SsdStaticFrame_initialize(&self->framesForInitializing[5], SEVENSEGDRIVER_BITPTN_LEFTDOWN,		250);
	SsdStaticFrame_initialize(&self->framesForInitializing[6], SEVENSEGDRIVER_BITPTN_DASH,			250);
	SsdStaticFrame_initialize(&self->framesForInitializing[7], SEVENSEGDRIVER_BITPTN_DOT,			250);
	SsdAnimation_initialize(&self->animationForInitializing);
	for(index = 0; index < cSsdRestoreMode_numofFramesForInitializing; index++)
	{
		SsdAnimation_addFrame(&self->animationForInitializing, (SsdFrame*)&self->framesForInitializing[index]);
	}

	// ForProcessing
	SsdStaticFrame_initialize(&self->leftFramesForProcessing[0], SEVENSEGDRIVER_BITPTN_UPCIRCLE,	250);
	SsdStaticFrame_initialize(&self->leftFramesForProcessing[1], SEVENSEGDRIVER_BITPTN_BLANK,		250);
	SsdStaticFrame_initialize(&self->leftFramesForProcessing[2], SEVENSEGDRIVER_BITPTN_BLANK,		250);
	SsdStaticFrame_initialize(&self->leftFramesForProcessing[3], SEVENSEGDRIVER_BITPTN_DOWNCIRCLE,	250);
	SsdStaticFrame_initialize(&self->rightFramesForProcessing[0], SEVENSEGDRIVER_BITPTN_BLANK,		250);
	SsdStaticFrame_initialize(&self->rightFramesForProcessing[1], SEVENSEGDRIVER_BITPTN_UPCIRCLE,	250);
	SsdStaticFrame_initialize(&self->rightFramesForProcessing[2], SEVENSEGDRIVER_BITPTN_DOWNCIRCLE,	250);
	SsdStaticFrame_initialize(&self->rightFramesForProcessing[3], SEVENSEGDRIVER_BITPTN_BLANK,		250);
	SsdAnimation_initialize(&self->leftAnimationForProcessing);
	SsdAnimation_initialize(&self->rightAnimationForProcessing);
	for(index = 0; index < cSsdRestoreMode_numofFramesForProcessing; index++)
	{
		SsdAnimation_addFrame(
			&self->leftAnimationForProcessing,
			(SsdFrame*)&self->leftFramesForProcessing[index]
			);
		SsdAnimation_addFrame(
			&self->rightAnimationForProcessing,
			(SsdFrame*)&self->rightFramesForProcessing[index]
			);
	}

	// ForSignature
	SsdDynamicFrame_initialize(
		&self->leftFramesForSignature[0],
		__SsdRestoreMode_get1stLeftPictureOfSignature,
		(void*)self,
		500
		);
	SsdDynamicFrame_initialize(
		&self->leftFramesForSignature[1],
		__SsdRestoreMode_get2ndLeftPictureOfSignature,
		(void*)self,
		500
		);
	SsdDynamicFrame_initialize(
		&self->rightFramesForSignature[0],
		__SsdRestoreMode_get1stRightPictureOfSignature,
		(void*)self,
		500
		);
	SsdDynamicFrame_initialize(
		&self->rightFramesForSignature[1],
		__SsdRestoreMode_get2ndRightPictureOfSignature,
		(void*)self,
		500
		);
	SsdAnimation_initialize(&self->leftAnimationForSignature);
	SsdAnimation_initialize(&self->rightAnimationForSignature);
	for(index = 0; index < cSsdRestoreMode_numofFramesForSignature; index++)
	{
		SsdAnimation_addFrame(
			&self->leftAnimationForSignature,
			(SsdFrame*)&self->leftFramesForSignature[index]
			);
		SsdAnimation_addFrame(
			&self->rightAnimationForSignature,
			(SsdFrame*)&self->rightFramesForSignature[index]
			);
	}


	// TODO: from HERE!!

	self->ssdDriver = ssdDriver;
}

//-------------------------------------------------------------------
// SsdModeTransitionEvent
//-------------------------------------------------------------------
typedef enum eSsdModeTransitionEvent eSsdModeTransitionEvent;
enum eSsdModeTransitionEvent
{
	eSsdModeTransitionEvent_onInitialized = 0,
	eSsdModeTransitionEvent_onRestore,
	eSsdModeTransitionEvent_onCriticalFault,
	eSsdModeTransitionEvent_onFaultOccured,
	eSsdModeTransitionEvent_onFaultRemoved,
	eSsdModeTransitionEvent_onServiceSwitchOn,
	eSsdModeTransitionEvent_onServiceSwitchServiceFinished,

	eSsdModeTransitionEvent_numofEvents,
};

//-------------------------------------------------------------------
// SsdModeTransition
//-------------------------------------------------------------------
typedef struct SsdModeTransition SsdModeTransition;
struct SsdModeTransition
{
	SsdModeState* src;
	SsdModeState* dst;
	eSsdModeTransitionEvent event;
};

//-------------------------------------------------------------------
void SsdModeTransition_initialize(
	SsdModeTransition* self,
	SsdModeState* src,
	SsdModeState* dst,
	eSsdModeTransitionEvent event
	)
{
	self->src = src;
	self->dst = dst;
	self->event = event;
}

//-------------------------------------------------------------------
Bool SsdModeTransition_canTransit(
	SsdModeTransition* self,
	SsdModeState* src,
	eSsdModeTransitionEvent event
	)
{
	if((self->src == src) && (self->event == event))
	{
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
SsdModeState* SsdModeTransition_getDstState(
	SsdModeTransition* self
	)
{
	return self->dst;
}

//-------------------------------------------------------------------
// SsdModeStateMachine
//-------------------------------------------------------------------
#define cSsdModeStateMachine_maxNumofTransitions 32
typedef struct SsdModeStateMachine SsdModeStateMachine;
struct SsdModeStateMachine
{
	SsdModeState* state;
	SsdModeTransition transitions[cSsdModeStateMachine_maxNumofTransitions];
	int numofTransitions;
};

//-------------------------------------------------------------------
void SsdModeStateMachine_initialize(
	SsdModeStateMachine* self
	)
{
	self->state = NULL;
	self->numofTransitions = 0;
}

//-------------------------------------------------------------------
void SsdModeStateMachine_setInitialState(
	SsdModeStateMachine* self,
	SsdModeState* initialState
	)
{
	self->state = initialState;
}

//-------------------------------------------------------------------
Bool SsdModeStateMachine_addTransition(
	SsdModeStateMachine* self,
	SsdModeState* srcState,
	SsdModeState* dstState,
	eSsdModeTransitionEvent event
	)
{
	int index;

	if(self->numofTransitions == cSsdModeStateMachine_maxNumofTransitions)
	{
		return FALSE;
	}

	SsdModeTransition_initialize(&self->transitions[self->numofTransitions], srcState, dstState, event);
	self->numofTransitions += 1;
	return TRUE;
}

//-------------------------------------------------------------------
static void __SsdModeStateMachine_lock(
	SsdModeStateMachine* self
	)
{
	// TODO	
}

//-------------------------------------------------------------------
static void __SsdModeStateMachine_unlock(
	SsdModeStateMachine* self
	)
{
	// TODO	
}

//-------------------------------------------------------------------
static void __SsdModeStateMachine_transit(
	SsdModeStateMachine* self,
	SsdModeState* dst
	)
{
	SsdModeState* src = self->state;

	SsdModeState_onExit(src);
	SsdModeState_onEntry(dst);

	self->state = dst;
}

//-------------------------------------------------------------------
void SsdModeStateMachine_onEvent(
	SsdModeStateMachine* self,
	eSsdModeTransitionEvent event
	)
{
	int index;

	__SsdModeStateMachine_lock(self);

	for(index = 0; index < self->numofTransitions; index++)
	{
		SsdModeTransition* transition = &self->transitions[index];

		if(SsdModeTransition_canTransit(transition, self->state, event))
		{
			SsdModeState* dst = SsdModeTransition_getDstState(transition);
			__SsdModeStateMachine_transit(self, dst);
			break;
		}
	}

	__SsdModeStateMachine_unlock(self);
}

//-------------------------------------------------------------------
void SsdModeStateMachine_onTick(
	SsdModeStateMachine* self,
	uint32_t elapsedTimeMillisec
	)
{
	__SsdModeStateMachine_lock(self);
	SsdModeState_onTick(self->state, elapsedTimeMillisec);
	__SsdModeStateMachine_unlock(self);
}

//-------------------------------------------------------------------
// SsdModeStateMachineFactory
//-------------------------------------------------------------------
typedef struct SsdModeStateMachineFactory SsdModeStateMachineFactory;
struct SsdModeStateMachineFactory
{
	SsdModeStateMachine stateMachine;
	SsdTestMode testMode;
};

//-------------------------------------------------------------------
SsdModeStateMachineFactory G_SsdModeStateMachineFactory;

//-------------------------------------------------------------------
SsdModeStateMachineFactory* SsdModeStateMachineFactory_getInstance(
	void
	)
{
	return &G_SsdModeStateMachineFactory;
}

//-------------------------------------------------------------------
SsdModeStateMachine* SsdModeStateMachineFactory_createStateMachine(
	StateEventMessanger* stateEventMessanger,
	ConfigEventMessanger* configEventMessanger,
	DataLoggingEventMessanger* dataLoggingEventMessanger,
	FaultLogWatcher* faultLogWatcher,
	SsdDriverWrapper* ssdDriver,
	ServiceSwitchDriverWrapper* serviceSwitchDriver
	)
{
	SsdModeStateMachineFactory* self = SsdModeStateMachineFactory_getInstance();
	SsdModeStateMachine* sm;
	SsdModeState* testMode;
	SsdModeState* normalMode;
	SsdModeState* faultMode;
	SsdModeState* serviceSwitchMode;
	SsdModeState* restoreMode;
	SsdModeState* criticalFaultMode;

	SsdTestMode_initialize(&self->testMode, ssdDriver);
	// TODO: initialize other states

	sm = &self->stateMachine;
	SsdModeStateMachine_initialize(sm);
	SsdModeStateMachine_setInitialState(sm, testMode);
	SsdModeStateMachine_addTransition(sm, testMode, normalMode,		eSsdModeTransitionEvent_onInitialized);
	SsdModeStateMachine_addTransition(sm, testMode, restoreMode,	eSsdModeTransitionEvent_onRestore);
	// ...
	SsdModeStateMachine_addTransition(sm, normalMode, faultMode,			eSsdModeTransitionEvent_onFaultOccured);
	SsdModeStateMachine_addTransition(sm, normalMode, serviceSwitchMode,	eSsdModeTransitionEvent_onServiceSwitchOn);
	SsdModeStateMachine_addTransition(sm, normalMode, criticalFaultMode,	eSsdModeTransitionEvent_onCriticalFault);

	return sm; // TODO
}

//-------------------------------------------------------------------
// SsdManager
//-------------------------------------------------------------------
typedef struct SsdManager SsdManager;
struct SsdManager
{
	// brokers relating external components
	ServiceSwitchDriverWrapper	serviceSwitchDriver;
	SsdDriverWrapper			ssdDriver;
	StateEventMessanger			stateEventMessanger;
	ConfigEventMessanger		configEventMessanger;
	DataLoggingEventMessanger	dataLoggingEventMessanger;
	FaultLogWatcher				faultLogWatcher;

	// internal objects
	SsdModeStateMachine* sm;
};

//-------------------------------------------------------------------
static SsdManager G_SsdManager;

//-------------------------------------------------------------------
static SsdManager* SsdManager_getInstance(
	void
	)
{
	return &G_SsdManager;
}

//-------------------------------------------------------------------
void SsdManager_cyclicTask(
	SsdManager* self
	)
{
	const uint32_t timeMillisecOfTimer = 50;

	SsdModeStateMachine_onTick(self->sm, timeMillisecOfTimer);
}

//-------------------------------------------------------------------
uint8_t SsdManager_initial(
	void
	)
{
	SsdManager* self = SsdManager_getInstance();

	//ServiceSwitchDriverWrapper_initialize(self->serviceSwitchDriver);
	//SsdDriverWrapper_initialize(self->serviceSwitchDriver);
	StateEventMessanger_initialize(&self->stateEventMessanger);
	ConfigEventMessanger_initialize(&self->configEventMessanger);
	DataLoggingEventMessanger_initialize(&self->dataLoggingEventMessanger);
	FaultLogWatcher_initialize(&self->faultLogWatcher);

	self->sm = SsdModeStateMachineFactory_createStateMachine(
					&self->stateEventMessanger,
					&self->configEventMessanger,
					&self->dataLoggingEventMessanger,
					&self->faultLogWatcher,
					&self->ssdDriver,
					&self->serviceSwitchDriver
					);

	return TRUE; // TODO
}

int main(void)
{
	printf("hello\n");
}
