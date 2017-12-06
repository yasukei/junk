#include <stdio.h>
#include <stdint.h>

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
	SsdDriverWrapper* self
	// TODO
	)
{
	// TODO
}

//-------------------------------------------------------------------
void SsdDriverWrapper_display(
	SsdDriverWrapper* self,
	uint8_t left,
	uint8_t right
	)
{
	// TODO
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
void StateEventMessanger_notifyUpdated(
	StateEventMessanger* self,
	int state
	)
{
	// TODO: lock & unlock
	self->state = state;
	EventMessanger_notifyUpdated(&self->eventMessanger);
}

//-------------------------------------------------------------------
Bool StateEventMessanger_subscribe(
	StateEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	// TODO: lock & unlock
	return EventMessanger_subscribe(&self->eventMessanger, callback, context);
}

//-------------------------------------------------------------------
int StateEventMessanger_getState(
	StateEventMessanger* self
	)
{
	// TODO: lock & unlock
	return self->state;
}

//-------------------------------------------------------------------
// ConfigEventMessangerSignature
//-------------------------------------------------------------------
typedef struct ConfigEventMessangerSignature ConfigEventMessangerSignature;
struct ConfigEventMessangerSignature
{
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

struct ConfigEventMessanger
{
	EventMessanger eventMessanger;
	Bool hasSignature;
	ConfigEventMessangerSignature signature;
};

//-------------------------------------------------------------------
void ConfigEventMessanger_initialize(
	ConfigEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	self->hasSignature = FALSE;
}

//-------------------------------------------------------------------
void ConfigEventMessanger_notifyUpdated(
	ConfigEventMessanger* self,
	ConfigEventMessangerSignature* signature
	)
{
	// TODO: lock & unlock
	self->signature = *signature;
	self->hasSignature = TRUE;
	EventMessanger_notifyUpdated(&self->eventMessanger);
}

//-------------------------------------------------------------------
Bool ConfigEventMessanger_subscribe(
	ConfigEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	// TODO: lock & unlock
	return EventMessanger_subscribe(&self->eventMessanger, callback, context);
}

//-------------------------------------------------------------------
Bool ConfigEventMessanger_hasSignature(
	ConfigEventMessanger* self
	)
{
	// TODO: lock & unlock
	return self->hasSignature;
}

//-------------------------------------------------------------------
void ConfigEventMessanger_getSignature(
	ConfigEventMessanger* self,
	ConfigEventMessangerSignature* signature	// out
	)
{
	// TODO: lock & unlock
	*signature = self->signature;
}

//-------------------------------------------------------------------
// DataLoggingEventMessangerDotPattern
//-------------------------------------------------------------------
typedef enum eDataLoggingEventMessangerDotPattern eDataLoggingEventMessangerDotPattern;

enum eDataLoggingEventMessangerDotPattern
{
	eDataLoggingEventMessangerDotPattern_on,
	eDataLoggingEventMessangerDotPattern_off,
	eDataLoggingEventMessangerDotPattern_blink,
};


//-------------------------------------------------------------------
// DataLoggingEventMessanger
//-------------------------------------------------------------------
typedef struct DataLoggingEventMessanger DataLoggingEventMessanger;

struct DataLoggingEventMessanger
{
	EventMessanger eventMessanger;
	eDataLoggingEventMessangerDotPattern left;
	eDataLoggingEventMessangerDotPattern right;
};

//-------------------------------------------------------------------
void DataLoggingEventMessanger_initialize(
	DataLoggingEventMessanger* self
	)
{
	EventMessanger_initialize(&self->eventMessanger);
	self->left = eDataLoggingEventMessangerDotPattern_off;
	self->right = eDataLoggingEventMessangerDotPattern_off;
}

//-------------------------------------------------------------------
void DataLoggingEventMessanger_notifyUpdated(
	DataLoggingEventMessanger* self,
	eDataLoggingEventMessangerDotPattern left,
	eDataLoggingEventMessangerDotPattern right
	)
{
	// TODO: lock & unlock
	self->left = left;
	self->right = right;
	EventMessanger_notifyUpdated(&self->eventMessanger);
}

//-------------------------------------------------------------------
Bool DataLoggingEventMessanger_subscribe(
	DataLoggingEventMessanger* self,
	EventMessangerCallback callback,
	void* context
	)
{
	// TODO: lock & unlock
	return EventMessanger_subscribe(&self->eventMessanger, callback, context);
}

//-------------------------------------------------------------------
void DataLoggingEventMessanger_getDotPattern(
	DataLoggingEventMessanger* self,
	eDataLoggingEventMessangerDotPattern* left,	// out
	eDataLoggingEventMessangerDotPattern* right	// out
	)
{
	// TODO: lock & unlock
	*left = self->left;
	*right = self->right;
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
static void __FaultLogWatcher_notifyUpdated(
	FaultLogWatcher* self
	)
{
	// TODO: lock & unlock
	EventMessanger_notifyUpdated(&self->eventMessanger);
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
	// TODO: lock & unlock
	return EventMessanger_subscribe(&self->eventMessanger, callback, context);
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
#define SsdPicture uint8_t

//-------------------------------------------------------------------
// SsdFrame
//-------------------------------------------------------------------
typedef struct SsdFrame SsdFrame;
struct SsdFrame
{
	int dummy; // TODO

	SsdFrame* next;	// for SsdAnimation
};

//-------------------------------------------------------------------
void SsdFrame_initialize(
	SsdFrame* self,
	SsdPicture picture,
	uint32_t durationTime
	)
{
	// TODO
}

//-------------------------------------------------------------------
SsdPicture SsdFrame_getPicture(
	SsdFrame* self
	)
{
	// TODO
	return 0;
}

//-------------------------------------------------------------------
uint32_t SsdFrame_getDurationTime(
	SsdFrame* self
	)
{
	// TODO
	return 0;
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
typedef struct SsdModeState SsdModeState;

typedef void (*SsdModeStateMethod_onEntry)(SsdModeState* self);
typedef void (*SsdModeStateMethod_onExit)(SsdModeState* self);
typedef void (*SsdModeStateMethod_onTick)(SsdModeState* self, uint32_t elapsedTimeMillisec);

struct SsdModeState
{
	SsdModeStateMethod_onEntry	onEntry;
	SsdModeStateMethod_onExit	onExit;
	SsdModeStateMethod_onTick	onTick;
};

//-------------------------------------------------------------------
void SsdModeState_initializeMethods(
	SsdModeState* self,
	SsdModeStateMethod_onEntry	onEntry,
	SsdModeStateMethod_onExit	onExit,
	SsdModeStateMethod_onTick	onTick
	)
{
	self->onEntry	= onEntry;
	self->onExit	= onExit;
	self->onTick	= onTick;
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
	self->onEntry(self);
}

//-------------------------------------------------------------------
void SsdModeState_onExit(
	SsdModeState* self
	)
{
	self->onExit(self);
}

//-------------------------------------------------------------------
void SsdModeState_onTick(
	SsdModeState* self,
	uint32_t elapsedTimeMillisec
	)
{
	self->onTick(self, elapsedTimeMillisec);
}

//-------------------------------------------------------------------
// SsdTestMode
//-------------------------------------------------------------------
typedef struct SsdTestMode SsdTestMode;

#define cSsdTestMode_numofFrames	9

struct SsdTestMode
{
	SsdModeState base;
	SsdFrame frames[cSsdTestMode_numofFrames];
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
	if(isFinished)
	{
		SsdAnimation_restartAnimation(&self->animation);
	}
	SsdDriverWrapper_display(self->ssdDriver, picture, picture);
}

//-------------------------------------------------------------------
void SsdTestMode_initialize(
	SsdTestMode* self,
	SsdDriverWrapper* ssdDriver
	)
{
	struct
	{
		SsdPicture picture;
		uint32_t durationTime;
	}
	initialValues[] =
	{
		// TODO: change proper value for SsdDriver
		{ 'F', 250},
		{ 'A', 250},
		{ 'B', 250},
		{ 'C', 250},
		{ 'D', 250},
		{ 'E', 250},
		{ 'G', 250},
		{ '.', 250},
		{ ' ', 250},
	};
	int index;

	SsdModeState_initializeMethods(
		(SsdModeState*)self,
		SsdTestMode_onEntry,
		SsdModeState_doNothing,
		SsdTestMode_onTick
		);

	SsdAnimation_initialize(&self->animation);
	for(index = 0; index < cSsdTestMode_numofFrames; index++)
	{
		SsdFrame_initialize(
			&self->frames[index],
			initialValues[index].picture,
			initialValues[index].durationTime
			);
		SsdAnimation_addFrame(&self->animation, &self->frames[index]);
	}

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
	// external dependents
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
