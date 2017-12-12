#include <stdio.h>
#include <stdint.h>

#include "./externals.c"

// TODO: delete
#define Bool	uint8_t
#define TRUE	((Bool)1)
#define FALSE	((Bool)0)

//-------------------------------------------------------------------
// SsdUtil
//-------------------------------------------------------------------
uint8_t SsdUtil_getUpper4bit(
	uint8_t byte
	)
{
	return (byte >> 4) & 0x0F;
}

//-------------------------------------------------------------------
uint8_t SsdUtil_getLower4bit(
	uint8_t byte
	)
{
	return (byte >> 0) & 0x0F;
}

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
	patternList.nextList = 0;
	
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
		return FALSE;	// TODO: guarantee not to come by test
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
	ConfigEventMessangerSignature signature;
	e_SsdDisplayCommandForRestore displayCommandForRestore;
};

//-------------------------------------------------------------------
void ConfigEventMessanger_initialize(
	ConfigEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
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

static const SsdPicture SsdPicture_hexTable[16] =
{
	SEVENSEGDRIVER_BITPTN_0,
	SEVENSEGDRIVER_BITPTN_1,
	SEVENSEGDRIVER_BITPTN_2,
	SEVENSEGDRIVER_BITPTN_3,
	SEVENSEGDRIVER_BITPTN_4,
	SEVENSEGDRIVER_BITPTN_5,
	SEVENSEGDRIVER_BITPTN_6,
	SEVENSEGDRIVER_BITPTN_7,
	SEVENSEGDRIVER_BITPTN_8,
	SEVENSEGDRIVER_BITPTN_9,
	SEVENSEGDRIVER_BITPTN_A,
	SEVENSEGDRIVER_BITPTN_B,
	SEVENSEGDRIVER_BITPTN_C,
	SEVENSEGDRIVER_BITPTN_D,
	SEVENSEGDRIVER_BITPTN_E,
	SEVENSEGDRIVER_BITPTN_F,
};

//-------------------------------------------------------------------
SsdPicture SsdPicture_convert4bitValueToHexPicture(
	uint8_t value
	)
{
	return SsdPicture_hexTable[ value & 0x0F ];
}

//-------------------------------------------------------------------
SsdPicture SsdPicture_getOnesDigitPictureAsDecimal(
	uint8_t value
	)
{
	return SsdPicture_hexTable[ value % 10 ];
}

//-------------------------------------------------------------------
SsdPicture SsdPicture_getTensDigitPictureAsDecimal(
	uint8_t value
	)
{
	return SsdPicture_hexTable[ (value / 10) % 10 ];
}

//-------------------------------------------------------------------
SsdPicture SsdPicture_getUpper4bitPictureAsHex(
	uint8_t value
	)
{
	return SsdPicture_convert4bitValueToHexPicture(SsdUtil_getUpper4bit(value));
}

//-------------------------------------------------------------------
SsdPicture SsdPicture_getLower4bitPictureAsHex(
	uint8_t value
	)
{
	return SsdPicture_convert4bitValueToHexPicture(SsdUtil_getLower4bit(value));
}

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
static void __SsdAnimation_changeCurrentFrame(
	SsdAnimation* self,
	SsdFrame* newFrame
	)
{
	self->currentFrame = newFrame;
	self->elapsedTimeMillisecOfCurrentFrame = 0;
}

//-------------------------------------------------------------------
void SsdAnimation_restartAnimation(
	SsdAnimation* self
	)
{
	__SsdAnimation_changeCurrentFrame(self, self->headFrame);
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
	SsdPicture result;

	nextFrame = self->currentFrame->next;
	if(nextFrame != NULL)
	{
		result = SsdFrame_getPicture(nextFrame);
		__SsdAnimation_changeCurrentFrame(self, nextFrame);
		*isFinished = FALSE;
	}
	else
	{
		result = SsdFrame_getPicture(self->currentFrame);
		SsdAnimation_restartAnimation(self);
		*isFinished = TRUE;
	}

	return result;
}

//-------------------------------------------------------------------
SsdPicture SsdAnimation_getPicture(
	SsdAnimation* self,
	uint32_t elapsedTimeMillisec,
	Bool* isFinished	// out
	)
{
	self->elapsedTimeMillisecOfCurrentFrame += elapsedTimeMillisec;

	if(self->elapsedTimeMillisecOfCurrentFrame < SsdFrame_getDurationTime(self->currentFrame))
	{
		*isFinished = FALSE;
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

	// onTick
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
	SsdPicture left;
	SsdPicture right;
	SsdPicture leftDot;
	SsdPicture rightDot;

	SsdAnimation_restartAnimation(self->leftAnimation);
	SsdAnimation_restartAnimation(self->rightAnimation);
	SsdAnimation_restartAnimation(self->leftDotAnimation);
	SsdAnimation_restartAnimation(self->rightDotAnimation);
	left		= SsdAnimation_getFirstPicture(self->leftAnimation);
	right		= SsdAnimation_getFirstPicture(self->rightAnimation);
	leftDot		= SsdAnimation_getFirstPicture(self->leftDotAnimation);
	rightDot	= SsdAnimation_getFirstPicture(self->rightDotAnimation);

	SsdDriverWrapper_display(self->ssdDriver, left | leftDot, right | rightDot);
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
	e_State_Program,
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
void __SsdNormalMode_chooseAnimationByState(
	SsdNormalMode* self,
	int state,
	SsdAnimation** left,	// out
	SsdAnimation** right	// out
	)
{
	switch(state)
	{
		case e_State_Run:
			*left	= &self->leftAnimationForRun;
			*right	= &self->rightAnimationForRun;
			break;
		case e_State_Debug:
			*left	= &self->leftAnimationForDebug;
			*right	= &self->rightAnimationForDebug;
			break;
		case e_State_Program:
			// fall through
		default:
			*left	= &self->leftAnimationForProgram;
			*right	= &self->rightAnimationForProgram;
			break;
	}
}

//-------------------------------------------------------------------
void __SsdNormalMode_notifyStateManagerUpdated(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	int state;
	SsdAnimation* leftAnim;
	SsdAnimation* rightAnim;

	state = StateEventMessanger_getState(self->stateEventMessanger);
	__SsdNormalMode_chooseAnimationByState(self, state, &leftAnim, &rightAnim);
	__SsdNormalMode_changeAnimation(self, leftAnim, rightAnim);
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
SsdAnimation* __SsdNormalMode_chooseDotAnimationByDotPattern(
	e_DataLoggingEventMessangerDotPattern dotPattern,
	SsdAnimation* onAnimation,
	SsdAnimation* offAnimation,
	SsdAnimation* blinkAnimation
	)
{
	SsdAnimation* result;

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

	leftDotAnimation = __SsdNormalMode_chooseDotAnimationByDotPattern(
									leftDotPattern,
									&self->dotOnAnimation,
									&self->dotOffAnimation,
									&self->leftDotBlinkAnimation
									);
	rightDotAnimation = __SsdNormalMode_chooseDotAnimationByDotPattern(
									rightDotPattern,
									&self->dotOnAnimation,
									&self->dotOffAnimation,
									&self->rightDotBlinkAnimation
									);
	__SsdNormalMode_changeDotAnimation(self, leftDotAnimation, rightDotAnimation);
}

//-------------------------------------------------------------------
SsdPicture __SsdNormalMode_getLeftPictureOfSignature(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	ConfigEventMessangerSignature signature;
	SsdPicture result;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(signature.valid)
	{
		result = SsdPicture_getUpper4bitPictureAsHex(signature.signatureLow);
	}
	else
	{
		result = SEVENSEGDRIVER_BITPTN_DASH;
	}
	return result;
}

//-------------------------------------------------------------------
SsdPicture __SsdNormalMode_getRightPictureOfSignature(
	void* context
	)
{
	SsdNormalMode* self = (SsdNormalMode*)context;
	ConfigEventMessangerSignature signature;
	SsdPicture result;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(signature.valid)
	{
		result = SsdPicture_getLower4bitPictureAsHex(signature.signatureLow);
	}
	else
	{
		result = SEVENSEGDRIVER_BITPTN_DASH;
	}
	return result;
}

//-------------------------------------------------------------------
void __SsdNormalMode_initializeAnimationForRun(
	SsdNormalMode* self
	)
{
	SsdDynamicFrame* lFrame = &self->leftFrameForRun;
	SsdDynamicFrame* rFrame = &self->rightFrameForRun;
	SsdAnimation* lAnim = &self->leftAnimationForRun;
	SsdAnimation* rAnim = &self->rightAnimationForRun;

	SsdDynamicFrame_initialize(lFrame, __SsdNormalMode_getLeftPictureOfSignature, (void*)self, 0);
	SsdDynamicFrame_initialize(rFrame, __SsdNormalMode_getRightPictureOfSignature, (void*)self, 0);
	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	SsdAnimation_addFrame(lAnim,		(SsdFrame*)lFrame);
	SsdAnimation_addFrame(rAnim,		(SsdFrame*)rFrame);
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

	__SsdNormalMode_changeAnimation(self, &self->leftAnimationForProgram, &self->rightAnimationForProgram);
	__SsdNormalMode_changeDotAnimation(self, &self->dotOffAnimation, &self->dotOffAnimation);

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
	SsdStaticFrame leftFramesForInitializing[cSsdRestoreMode_numofFramesForInitializing];
	SsdStaticFrame rightFramesForInitializing[cSsdRestoreMode_numofFramesForInitializing];
	SsdAnimation leftAnimationForInitializing;
	SsdAnimation rightAnimationForInitializing;

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
	SsdDynamicFrame leftFramesForSignatureWithDot[cSsdRestoreMode_numofFramesForSignatureWithDot];
	SsdDynamicFrame rightFramesForSignatureWithDot[cSsdRestoreMode_numofFramesForSignatureWithDot];
	SsdAnimation leftAnimationForSignatureWithDot;
	SsdAnimation rightAnimationForSignatureWithDot;

	// ForSignatureWithDotAndDate
	SsdDynamicFrame leftFramesForSignatureWithDotAndDate[cSsdRestoreMode_numofFramesForSignatureWithDotAndDate];
	SsdDynamicFrame rightFramesForSignatureWithDotAndDate[cSsdRestoreMode_numofFramesForSignatureWithDotAndDate];
	SsdAnimation leftAnimationForSignatureWithDotAndDate;
	SsdAnimation rightAnimationForSignatureWithDotAndDate;

	// onTick
	SsdAnimation* leftAnimation;
	SsdAnimation* rightAnimation;

	SsdDriverWrapper* ssdDriver;
	ConfigEventMessanger* configEventMessanger;
};

//-------------------------------------------------------------------
static void __SsdRestoreMode_lock(
	SsdRestoreMode* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_unlock(
	SsdRestoreMode* self
	)
{
	// TODO
}

//-------------------------------------------------------------------
void SsdRestoreMode_onEntry(
	SsdModeState* base
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)base;
	SsdPicture leftPicture;
	SsdPicture rightPicture;

	SsdAnimation_restartAnimation(self->leftAnimation);
	SsdAnimation_restartAnimation(self->rightAnimation);
	leftPicture = SsdAnimation_getFirstPicture(self->leftAnimation);
	rightPicture = SsdAnimation_getFirstPicture(self->rightAnimation);
	SsdDriverWrapper_display(self->ssdDriver, leftPicture, rightPicture);
}

//-------------------------------------------------------------------
void SsdRestoreMode_onTick(
	SsdModeState* base,
	uint32_t elapsedTimeMillisec
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)base;
	SsdPicture leftPicture;
	SsdPicture rightPicture;
	Bool dummy;

	__SsdRestoreMode_lock(self);
	leftPicture = SsdAnimation_getPicture(self->leftAnimation, elapsedTimeMillisec, &dummy);
	rightPicture = SsdAnimation_getPicture(self->rightAnimation, elapsedTimeMillisec, &dummy);
	__SsdRestoreMode_unlock(self);

	SsdDriverWrapper_display(self->ssdDriver, leftPicture, rightPicture);
}

//-------------------------------------------------------------------
SsdModeStateMethods G_SsdRestoreModeMethods =
{
	SsdRestoreMode_onEntry,
	SsdModeState_doNothing,
	SsdRestoreMode_onTick,
};

//-------------------------------------------------------------------
static void __SsdRestoreMode_changeAnimation(
	SsdRestoreMode* self,
	SsdAnimation* leftAnimation,
	SsdAnimation* rightAnimation
	)
{
	__SsdRestoreMode_lock(self);
	self->leftAnimation = leftAnimation;
	self->rightAnimation = rightAnimation;
	SsdAnimation_restartAnimation(self->leftAnimation);
	SsdAnimation_restartAnimation(self->rightAnimation);
	__SsdRestoreMode_unlock(self);
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_chooseAnimationByDisplayCommand(
	SsdRestoreMode* self,
	e_SsdDisplayCommandForRestore displayCommand,
	SsdAnimation** left,	// out
	SsdAnimation** right	// out
	)
{
	switch(displayCommand)
	{
		case e_SsdDisplayCommandForRestore_displayProcessing:
			*left	= &self->leftAnimationForProcessing;
			*right	= &self->rightAnimationForProcessing;
			break;
		case e_SsdDisplayCommandForRestore_displaySignature:
			*left	= &self->leftAnimationForSignature;
			*right	= &self->rightAnimationForSignature;
			break;
		case e_SsdDisplayCommandForRestore_displaySignatureWithDot:
			*left	= &self->leftAnimationForSignatureWithDot;
			*right	= &self->rightAnimationForSignatureWithDot;
			break;
		case e_SsdDisplayCommandForRestore_displaySignatureWithDotAndDate:
			*left	= &self->leftAnimationForSignatureWithDotAndDate;
			*right	= &self->rightAnimationForSignatureWithDotAndDate;
			break;
		case e_SsdDisplayCommandForRestore_displayInitializing:
			// fall through
		default:
			*left	= &self->leftAnimationForInitializing;
			*right	= &self->rightAnimationForInitializing;
			break;
	}
}

//-------------------------------------------------------------------
void __SsdRestoreMode_notifyConfigManagerUpdated(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;
	e_SsdDisplayCommandForRestore displayCommand;
	SsdAnimation* leftAnim;
	SsdAnimation* rightAnim;

	displayCommand = ConfigEventMessanger_getDisplayCommandForRestore(self->configEventMessanger);
	__SsdRestoreMode_chooseAnimationByDisplayCommand(self, displayCommand, &leftAnim, &rightAnim);
	__SsdRestoreMode_changeAnimation(self, leftAnim, rightAnim);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfSignature(
	SsdRestoreMode* self,
	Bool isSignatureHigh,	// if false, this means signatureLow
	Bool isLeft				// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;
	uint8_t signature1Byte;
	SsdPicture result;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(isSignatureHigh)
	{
		signature1Byte = signature.signatureHigh;
	}
	else
	{
		signature1Byte = signature.signatureLow;
	}

	if(isLeft)
	{
		result = SsdPicture_getUpper4bitPictureAsHex(signature1Byte);
	}
	else
	{
		result = SsdPicture_getLower4bitPictureAsHex(signature1Byte);
	}
	return result;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftOrRightPictureAsDecimal(
	uint8_t value,
	Bool isLeft			// if false, this means right
	)
{
	SsdPicture result;

	if(isLeft)
	{
		result = SsdPicture_getTensDigitPictureAsDecimal(value);
	}
	else
	{
		result = SsdPicture_getOnesDigitPictureAsDecimal(value);
	}
	return result;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfYear(
	SsdRestoreMode* self,
	Bool isYearHigh,	// if false, this means yearLow
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;
	uint8_t year1Byte;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);
	if(isYearHigh)
	{
		year1Byte = signature.yearHigh;
	}
	else
	{
		year1Byte = signature.yearLow;
	}

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(year1Byte, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfMonth(
	SsdRestoreMode* self,
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(signature.month, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfDay(
	SsdRestoreMode* self,
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(signature.day, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfHour(
	SsdRestoreMode* self,
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(signature.hour, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfMin(
	SsdRestoreMode* self,
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(signature.min, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getPictureOfSec(
	SsdRestoreMode* self,
	Bool isLeft			// if false, this means right
	)
{
	ConfigEventMessangerSignature signature;

	ConfigEventMessanger_getSignature(self->configEventMessanger, &signature);

	return __SsdRestoreMode_getLeftOrRightPictureAsDecimal(signature.sec, isLeft);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfSignatureHigh(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSignature(self, TRUE, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfSignatureHigh(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSignature(self, TRUE, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfSignatureLow(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSignature(self, FALSE, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfSignatureLow(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSignature(self, FALSE, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfSignatureHighWithDot(
	void* context
	)
{
	return __SsdRestoreMode_getLeftPictureOfSignatureHigh(context)
			| SEVENSEGDRIVER_BITPTN_DOT;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfSignatureHighWithDot(
	void* context
	)
{
	return __SsdRestoreMode_getRightPictureOfSignatureHigh(context)
			| SEVENSEGDRIVER_BITPTN_DOT;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfSignatureLowWithDot(
	void* context
	)
{
	return __SsdRestoreMode_getLeftPictureOfSignatureLow(context)
			| SEVENSEGDRIVER_BITPTN_DOT;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfSignatureLowWithDot(
	void* context
	)
{
	return __SsdRestoreMode_getRightPictureOfSignatureLow(context)
			| SEVENSEGDRIVER_BITPTN_DOT;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfYearHigh(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfYear(self, TRUE, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfYearHigh(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfYear(self, TRUE, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfYearLow(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfYear(self, FALSE, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfYearLow(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfYear(self, FALSE, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfMonth(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfMonth(self, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfMonth(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfMonth(self, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfDay(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfDay(self, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfDay(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfDay(self, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfHour(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfHour(self, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfHour(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfHour(self, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfMin(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfMin(self, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfMin(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfMin(self, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getLeftPictureOfSec(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSec(self, TRUE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getRightPictureOfSec(
	void* context
	)
{
	SsdRestoreMode* self = (SsdRestoreMode*)context;

	return __SsdRestoreMode_getPictureOfSec(self, FALSE);
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getDashPicture(
	void* context
	)
{
	(void)context;
	return SEVENSEGDRIVER_BITPTN_DASH;
}

//-------------------------------------------------------------------
static SsdPicture __SsdRestoreMode_getBlankPicture(
	void* context
	)
{
	(void)context;
	return SEVENSEGDRIVER_BITPTN_BLANK;
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_initializeAnimationForInitializing(
	SsdRestoreMode* self
	)
{
	SsdStaticFrame* lFrames = self->leftFramesForInitializing;
	SsdStaticFrame* rFrames = self->rightFramesForInitializing;
	SsdAnimation* lAnim = &self->leftAnimationForInitializing;
	SsdAnimation* rAnim = &self->rightAnimationForInitializing;
	int index;

	SsdStaticFrame_initialize(&lFrames[0], SEVENSEGDRIVER_BITPTN_LEFTUP,	250);
	SsdStaticFrame_initialize(&lFrames[1], SEVENSEGDRIVER_BITPTN_TOP,		250);
	SsdStaticFrame_initialize(&lFrames[2], SEVENSEGDRIVER_BITPTN_RIGHTUP,	250);
	SsdStaticFrame_initialize(&lFrames[3], SEVENSEGDRIVER_BITPTN_RIGHTDOWN,	250);
	SsdStaticFrame_initialize(&lFrames[4], SEVENSEGDRIVER_BITPTN_BOTTOM,	250);
	SsdStaticFrame_initialize(&lFrames[5], SEVENSEGDRIVER_BITPTN_LEFTDOWN,	250);
	SsdStaticFrame_initialize(&lFrames[6], SEVENSEGDRIVER_BITPTN_DASH,		250);
	SsdStaticFrame_initialize(&lFrames[7], SEVENSEGDRIVER_BITPTN_DOT,		250);

	SsdStaticFrame_initialize(&rFrames[0], SEVENSEGDRIVER_BITPTN_LEFTUP,	250);
	SsdStaticFrame_initialize(&rFrames[1], SEVENSEGDRIVER_BITPTN_TOP,		250);
	SsdStaticFrame_initialize(&rFrames[2], SEVENSEGDRIVER_BITPTN_RIGHTUP,	250);
	SsdStaticFrame_initialize(&rFrames[3], SEVENSEGDRIVER_BITPTN_RIGHTDOWN,	250);
	SsdStaticFrame_initialize(&rFrames[4], SEVENSEGDRIVER_BITPTN_BOTTOM,	250);
	SsdStaticFrame_initialize(&rFrames[5], SEVENSEGDRIVER_BITPTN_LEFTDOWN,	250);
	SsdStaticFrame_initialize(&rFrames[6], SEVENSEGDRIVER_BITPTN_DASH,		250);
	SsdStaticFrame_initialize(&rFrames[7], SEVENSEGDRIVER_BITPTN_DOT,		250);

	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	for(index = 0; index < cSsdRestoreMode_numofFramesForInitializing; index++)
	{
		SsdAnimation_addFrame(lAnim, (SsdFrame*)&lFrames[index]);
		SsdAnimation_addFrame(rAnim, (SsdFrame*)&rFrames[index]);
	}
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_initializeAnimationForProcessing(
	SsdRestoreMode* self
	)
{
	SsdStaticFrame* lFrames = self->leftFramesForProcessing;
	SsdStaticFrame* rFrames = self->rightFramesForProcessing;
	SsdAnimation* lAnim = &self->leftAnimationForProcessing;
	SsdAnimation* rAnim = &self->rightAnimationForProcessing;
	int index;

	SsdStaticFrame_initialize(&lFrames[0], SEVENSEGDRIVER_BITPTN_UPCIRCLE,		250);
	SsdStaticFrame_initialize(&lFrames[1], SEVENSEGDRIVER_BITPTN_BLANK,			250);
	SsdStaticFrame_initialize(&lFrames[2], SEVENSEGDRIVER_BITPTN_BLANK,			250);
	SsdStaticFrame_initialize(&lFrames[3], SEVENSEGDRIVER_BITPTN_DOWNCIRCLE,	250);

	SsdStaticFrame_initialize(&rFrames[0], SEVENSEGDRIVER_BITPTN_BLANK,			250);
	SsdStaticFrame_initialize(&rFrames[1], SEVENSEGDRIVER_BITPTN_UPCIRCLE,		250);
	SsdStaticFrame_initialize(&rFrames[2], SEVENSEGDRIVER_BITPTN_DOWNCIRCLE,	250);
	SsdStaticFrame_initialize(&rFrames[3], SEVENSEGDRIVER_BITPTN_BLANK,			250);

	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	for(index = 0; index < cSsdRestoreMode_numofFramesForProcessing; index++)
	{
		SsdAnimation_addFrame(lAnim, (SsdFrame*)&lFrames[index]);
		SsdAnimation_addFrame(rAnim, (SsdFrame*)&rFrames[index]);
	}
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_initializeAnimationForSignature(
	SsdRestoreMode* self
	)
{
	SsdDynamicFrame* lFrames = self->leftFramesForSignature;
	SsdDynamicFrame* rFrames = self->rightFramesForSignature;
	SsdAnimation* lAnim = &self->leftAnimationForSignature;
	SsdAnimation* rAnim = &self->rightAnimationForSignature;
	int index;

	SsdDynamicFrame_initialize(&lFrames[0], __SsdRestoreMode_getLeftPictureOfSignatureHigh,		(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[1], __SsdRestoreMode_getLeftPictureOfSignatureLow,		(void*)self, 500);

	SsdDynamicFrame_initialize(&rFrames[0], __SsdRestoreMode_getRightPictureOfSignatureHigh,	(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[1], __SsdRestoreMode_getRightPictureOfSignatureLow,		(void*)self, 500);

	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	for(index = 0; index < cSsdRestoreMode_numofFramesForSignature; index++)
	{
		SsdAnimation_addFrame(lAnim, (SsdFrame*)&lFrames[index]);
		SsdAnimation_addFrame(rAnim, (SsdFrame*)&rFrames[index]);
	}
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_initializeAnimationForSignatureWithDot(
	SsdRestoreMode* self
	)
{
	SsdDynamicFrame* lFrames = self->leftFramesForSignatureWithDot;
	SsdDynamicFrame* rFrames = self->rightFramesForSignatureWithDot;
	SsdAnimation* lAnim = &self->leftAnimationForSignatureWithDot;
	SsdAnimation* rAnim = &self->rightAnimationForSignatureWithDot;
	int index;

	SsdDynamicFrame_initialize(&lFrames[0], __SsdRestoreMode_getLeftPictureOfSignatureHighWithDot,	(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[1], __SsdRestoreMode_getLeftPictureOfSignatureLowWithDot,	(void*)self, 500);

	SsdDynamicFrame_initialize(&rFrames[0], __SsdRestoreMode_getRightPictureOfSignatureHigh,		(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[1], __SsdRestoreMode_getRightPictureOfSignatureLow,			(void*)self, 500);

	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	for(index = 0; index < cSsdRestoreMode_numofFramesForSignatureWithDot; index++)
	{
		SsdAnimation_addFrame(lAnim, (SsdFrame*)&lFrames[index]);
		SsdAnimation_addFrame(rAnim, (SsdFrame*)&rFrames[index]);
	}
}

//-------------------------------------------------------------------
static void __SsdRestoreMode_initializeAnimationForSignatureWithDotAndDate(
	SsdRestoreMode* self
	)
{
	SsdDynamicFrame* lFrames = self->leftFramesForSignatureWithDotAndDate;
	SsdDynamicFrame* rFrames = self->rightFramesForSignatureWithDotAndDate;
	SsdAnimation* lAnim = &self->leftAnimationForSignatureWithDotAndDate;
	SsdAnimation* rAnim = &self->rightAnimationForSignatureWithDotAndDate;
	int index;

	SsdDynamicFrame_initialize(&lFrames[0], __SsdRestoreMode_getLeftPictureOfSignatureHighWithDot,	(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[1], __SsdRestoreMode_getLeftPictureOfSignatureLowWithDot,	(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[2], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[3], __SsdRestoreMode_getLeftPictureOfYearHigh,				(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[4], __SsdRestoreMode_getLeftPictureOfYearLow,				(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[5], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[6], __SsdRestoreMode_getLeftPictureOfMonth,					(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[7], __SsdRestoreMode_getLeftPictureOfDay,					(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[8], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[9], __SsdRestoreMode_getLeftPictureOfHour,					(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[10], __SsdRestoreMode_getLeftPictureOfMin,					(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[11], __SsdRestoreMode_getLeftPictureOfSec,					(void*)self, 500);
	SsdDynamicFrame_initialize(&lFrames[12], __SsdRestoreMode_getBlankPicture,						(void*)self, 500);

	SsdDynamicFrame_initialize(&rFrames[0], __SsdRestoreMode_getRightPictureOfSignatureHighWithDot,	(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[1], __SsdRestoreMode_getRightPictureOfSignatureLowWithDot,	(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[2], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[3], __SsdRestoreMode_getRightPictureOfYearHigh,				(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[4], __SsdRestoreMode_getRightPictureOfYearLow,				(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[5], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[6], __SsdRestoreMode_getRightPictureOfMonth,				(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[7], __SsdRestoreMode_getRightPictureOfDay,					(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[8], __SsdRestoreMode_getDashPicture,						(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[9], __SsdRestoreMode_getRightPictureOfHour,					(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[10], __SsdRestoreMode_getRightPictureOfMin,					(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[11], __SsdRestoreMode_getRightPictureOfSec,					(void*)self, 500);
	SsdDynamicFrame_initialize(&rFrames[12], __SsdRestoreMode_getBlankPicture,						(void*)self, 500);

	SsdAnimation_initialize(lAnim);
	SsdAnimation_initialize(rAnim);
	for(index = 0; index < cSsdRestoreMode_numofFramesForSignatureWithDotAndDate; index++)
	{
		SsdAnimation_addFrame(lAnim, (SsdFrame*)&lFrames[index]);
		SsdAnimation_addFrame(rAnim, (SsdFrame*)&rFrames[index]);
	}
}

//-------------------------------------------------------------------
void SsdRestoreMode_initialize(
	SsdRestoreMode* self,
	SsdDriverWrapper* ssdDriver,
	ConfigEventMessanger* configEventMessanger
	)
{
	SsdModeState_initializeMethods(
		(SsdModeState*)self,
		&G_SsdRestoreModeMethods
		);

	__SsdRestoreMode_initializeAnimationForInitializing(self);
	__SsdRestoreMode_initializeAnimationForProcessing(self);
	__SsdRestoreMode_initializeAnimationForSignature(self);
	__SsdRestoreMode_initializeAnimationForSignatureWithDot(self);
	__SsdRestoreMode_initializeAnimationForSignatureWithDotAndDate(self);

	__SsdRestoreMode_changeAnimation(self, &self->leftAnimationForInitializing, &self->rightAnimationForInitializing);

	ConfigEventMessanger_subscribe(configEventMessanger, __SsdRestoreMode_notifyConfigManagerUpdated, (void*)self);

	self->ssdDriver = ssdDriver;
	self->configEventMessanger = configEventMessanger;
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
	return (self->src == src) && (self->event == event);
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
		return FALSE;	// TODO: guarantee not to come by test
	}

	SsdModeTransition_initialize(&self->transitions[self->numofTransitions], srcState, dstState, event);
	self->numofTransitions += 1;
	return TRUE;
}

//-------------------------------------------------------------------
void SsdModeStateMachine_start(
	SsdModeStateMachine* self
	)
{
	SsdModeState_onEntry(self->state);
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

	SsdModeStateMachine_start(sm);	// TODO: call this by SsdManager
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
