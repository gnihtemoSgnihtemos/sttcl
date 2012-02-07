/*
 * DemoStateB2.h
 *
 *  Created on: 31.12.2011
 *      Author: Admin
 */

#ifndef DEMOSTATEB2_H_
#define DEMOSTATEB2_H_

#include "State.h"
#include "RegionB.h"

namespace Application
{

class DemoStateB2
: public sttcl::State<DemoStateB2,RegionB,IConcurrentStateMachine>
{
public:
	DemoStateB2();
	virtual ~DemoStateB2();

	virtual void handleEvent1(IConcurrentStateMachine::Context* context,const void* eventArgs = 0);
	virtual void handleEvent2(IConcurrentStateMachine::Context* context,const void* eventArgs = 0);
	virtual void handleEvent3(IConcurrentStateMachine::Context* context,const void* eventArgs = 0);
	virtual void handleEvent4(IConcurrentStateMachine::Context* context,const void* eventArgs = 0);

	void entryImpl(IConcurrentStateMachine::Context* context);
	void exitImpl(IConcurrentStateMachine::Context* context);

	static DemoStateB2& getInstance();
};

}

#endif /* DEMOSTATEB2_H_ */