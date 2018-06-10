#pragma once

#include <stdlib.h>
#include <string.h>
#include <vector>

using ::std::vector;
class Event
{
public:
	Event(const char* name)
	{
		name_ = strdup(name);
	}
	~Event()
	{
		free(name_);
	}

	const char* getName() { return name_; }

private:
	char* name_;
};

class State
{
public:
	State() {};
	virtual ~State() {};

	virtual void entry() {};
	virtual void exit() {};
};

class StateMachine
{
public:
	StateMachine() :
		transitions_(), currentState_(nullptr)
	{};

	~StateMachine()
	{
		for(Transition* t : transitions_)
		{
			delete t;
		}
	};

	void setInitialState(State& initialState)
	{
		currentState_ = &initialState;
	}

	void start()
	{
		currentState_->entry();
	}

	void addTransition(State& src, State& dst, Event& event)
	{
		auto t = new Transition(&src, &dst, &event);

		transitions_.push_back(t);
	}

	void notify(Event& event)
	{
		for(Transition* t : transitions_)
		{
			if(t->canTransit(currentState_, &event))
			{
				transit(t->getDstState());
			}
		}
	}

private:
	class Transition
	{
	public:
		Transition(State* src, State* dst, Event* event) :
			src_(src), dst_(dst), event_(event)
		{
		}
		~Transition() {};

		State* getDstState()
		{
			return dst_;
		}

		bool canTransit(State* src, Event* event)
		{
			return src == src_ && event == event_;
		}

	private:
		State* src_;
		State* dst_;
		Event* event_;
	};

	vector<Transition*> transitions_;
	State* currentState_;

	void transit(State* dst)
	{
		currentState_->exit();
		dst->entry();
		currentState_ = dst;
	}
		
};
