#include <stdio.h>

//-------------------------------------------------------------------
// SsdModeState
//-------------------------------------------------------------------
typedef struct SsdModeState SsdModeState;
struct SsdModeState
{
	int dummy; // TODO: how to implement interface in C?
};

#define SsdModeState_onEntry(self)	( (self)->onEntry( (self) ) )
#define SsdModeState_onExit(self)	( (self)->onExit( (self) ) )
#define SsdModeState_onTick(self, elapsedTimeMillisec)	( (self)->onTick( (self), (elapsedTimeMillisec) ) )

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
	SsdState src;
	SsdState dst;
	eSsdModeTransitionEvent event
};

//-------------------------------------------------------------------
// SsdModeStateMachine
//-------------------------------------------------------------------
#define cSsdModeStateMachine_maxNumofTransitions 32
typedef struct SsdModeStateMachine SsdModeStateMachine;
struct SsdModeStateMachine
{
	SsdModeState state;
	SsdModeTransition transitions[cSsdModeStateMachine_maxNumofTransitions];
	int numofTransitions;
};

//-------------------------------------------------------------------
void SsdModeStateMachine_initialize(
	SsdModeStateMachine* self
	)
{
	self->state = NULL;
	numofTransitions = 0;
}

//-------------------------------------------------------------------
void SsdModeStateMachine_setInitialState(
	SsdModeStateMachine* self,
	SsdModeState* initialState
	)
{
	self->state = initialState
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
	return TRUE
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
		SsdModeTransition transition = &self->transitions[index];

		if(SsdModeTransition_canTransit(transition, self->state, event))
		{
			SsdModeState dstState = SsdModeTransition_getDstState(transition);
			__SsdModeStateMachine_transit(self, dstState);
			break;
		}
	}

	__SsdModeStateMachine_unlock(self);
}

//-------------------------------------------------------------------
static void __SsdModeStateMachine_transit(
	SsdModeStateMachine* self,
	SsdModeState* dstState,
	)
{
	SsdModeState_onExit(self->state);
	SsdModeState_onEntry(dstState);
	self->state = dstState;
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
// SsdManager
//-------------------------------------------------------------------
uint8_t SsdManager_initial(
	void
	)
{
	return TRUE; // TODO
}

//-------------------------------------------------------------------
typedef struct SsdManager SsdManager;
struct SsdManager
{
	SsdModeStateMachine stateMachine;
};

//-------------------------------------------------------------------
static SsdManager* __SsdManager_getInstance(
	void
	)
{
	
}

//-------------------------------------------------------------------
Bool SsdManager_initialize(
	StateManager* stateManager,
	ConfigManager* configManager,
	EventManager* eventManager,
	LoggingManager* loggingManager,
	ServiceSwitchDriver* serviceSwitchDriver,
	SsdDriver* ssdDriver
	)
{
	SsdManager* self;
	SsdModeState* testMode;
	SsdModeState* normalMode;
	SsdModeState* faultMode;
	SsdModeState* serviceSwitchMode;
	SsdModeState* restoreMode;
	SsdModeState* criticalFaultMode;

	SsdModeStateMachine_initialize(&self->stateMachine);

	return TRUE; // TODO
}

int main(void)
{
	printf("hello\n");
}
