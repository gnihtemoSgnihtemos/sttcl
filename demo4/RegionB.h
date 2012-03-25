/*
 * RegionB.h
 *
 *  Created on: 31.12.2011
 *      Author: Admin
 */

#ifndef REGIONB_H_
#define REGIONB_H_

#include "Region.h"
#include "IConcurrentStatemachine.h"
#include "DemoStateMachine.h"

namespace Application
{

class DemoStateMachine;
class ConcurrentStateMachine;

class RegionB
: public sttcl::Region<RegionB,DemoStateMachine,IConcurrentStateMachine>
{
public:
	typedef sttcl::Region<RegionB,DemoStateMachine,IConcurrentStateMachine> RegionBaseClass;
	typedef typename RegionBaseClass::InnerStateClass InnerStateClass;
	typedef ConcurrentStateMachine RegionContainerClass;

	RegionB(RegionContainerClass* regionContainer);
	virtual ~RegionB();

	virtual void handleEvent1(IDemoState::Context* context);
	virtual void handleEvent2(IDemoState::Context* context);
	virtual void handleEvent3(IDemoState::Context* context);
	virtual void handleEvent4(IDemoState::Context* context);

	void entryImpl(DemoStateMachine* context);
	void exitImpl(DemoStateMachine* context);

	void startingRegionThread();
	void endingRegionThread();

    bool initializeImpl(bool force);
    void finalizeImpl(bool finalizeSubStateMachines);

	InnerStateClass* getInitialStateImpl() const;

};

}

#endif /* REGIONB_H_ */
