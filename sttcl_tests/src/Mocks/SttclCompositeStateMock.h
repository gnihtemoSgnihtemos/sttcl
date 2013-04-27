/*
 * SttclCompositeStateMock.h
 *
 *  Created on: Apr 15, 2013
 *      Author: user
 */

#ifndef STTCLCOMPOSITESTATEMOCK_H_
#define STTCLCOMPOSITESTATEMOCK_H_

#include <iostream>
#include <gmock/gmock.h>
#include <CompositeState.h>
#include <State.h>
#include <SttclMutex.h>
#include <SttclTime.h>
#include "ITestStateInterface.h"
#include "ITestInnerStateInterface.h"
#include "ICompositeStateHooks.h"
#include "SttclTestLog.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;

template
    < class CompositeStateImpl
    , class StateMachineContext
    , class InnerStateInterface
    >
class SttclCompositeStateMock
: public sttcl::CompositeState<CompositeStateImpl,StateMachineContext,InnerStateInterface>
, public ICompositeStateHooks<CompositeStateImpl,StateMachineContext,InnerStateInterface>
{
public:

	typedef sttcl::CompositeState<CompositeStateImpl,StateMachineContext,InnerStateInterface> StateMachineBaseClass;
    typedef typename StateMachineBaseClass::StateImplementationBase StateBaseClass;
    typedef typename StateMachineBaseClass::InnerStateClass InnerStateClass;

	SttclCompositeStateMock(const std::string& stateMachineId = "<anonymous>", InnerStateClass* initialState = NULL)
	: initialState_(initialState)
	, enableLogs_(false)
	, stateMachineId_(stateMachineId)
    , directTransitState_(NULL)
    , finalizeOnNextDirectTransitCheck_(false)
	{
		ON_CALL(*this, initializeImpl(_))
			.WillByDefault(Invoke(this, &SttclCompositeStateMock::initializeImplCall));
		ON_CALL(*this, finalizeImpl(_))
			.WillByDefault(Invoke(this, &SttclCompositeStateMock::finalizeImplCall));
		ON_CALL(*this, subStateMachineCompleted())
			.WillByDefault(Invoke(this, &SttclCompositeStateMock::subStateMachineCompletedCall));
        ON_CALL(*this, subStateMachineCompletedImpl())
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::subStateMachineCompletedImplCall0));
		ON_CALL(*this, getInitialStateImpl())
			.WillByDefault(Invoke((const SttclCompositeStateMock*)this, &SttclCompositeStateMock::getInitialStateImplCall));
		ON_CALL(*this, isReadyImpl())
			.WillByDefault(Invoke((const SttclCompositeStateMock*)this, &SttclCompositeStateMock::isReadyImplCall));

        ON_CALL(*this, entryImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::entryImplCall));
        ON_CALL(*this, exitImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::exitImplCall));
        ON_CALL(*this, startDoImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::startDoImplCall));
        ON_CALL(*this, endDoImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::endDoImplCall));
        ON_CALL(*this, finalizeSubStateMachinesImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::finalizeSubStateMachinesImplCall));
        ON_CALL(*this, initSubStateMachinesImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::initSubStateMachinesImplCall));
        ON_CALL(*this, checkDirectTransitionImpl(_,_,_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::checkDirectTransitionImplCall));
        ON_CALL(*this, doActionImpl(_,_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::doActionImplCall));
        ON_CALL(*this, joinDoActionImpl(_))
            .WillByDefault(Invoke(this, &SttclCompositeStateMock::joinDoActionImplCall));
}

	virtual ~SttclCompositeStateMock()
	{
	}

	const std::string& id() const { return stateMachineId_; }

    void enableLogs(bool enable)
    {
        enableLogs_ = enable;
    }

    bool logsEnabled() const
    {
        return enableLogs_;
    }

	void setInitialState(InnerStateClass* initialState)
	{
    	sttcl::internal::AutoLocker<sttcl::internal::SttclMutex<> > lock(internalGuard_);
		initialState_ = initialState;
	}

    void setDirectTransitState(typename StateBaseClass::StateBaseType* directTransitState)
    {
        sttcl::internal::AutoLocker<sttcl::internal::SttclMutex<> > lock(internalGuard_);
        directTransitState_ = directTransitState;
    }

    void finalizeOnNextDirectTransitCheck()
    {
        sttcl::internal::AutoLocker<sttcl::internal::SttclMutex<> > lock(internalGuard_);
        finalizeOnNextDirectTransitCheck_ = true;
        directTransitState_ = NULL;
    }

	// ITestStateInterface
    MOCK_METHOD1_T(handleEvent1, void (StateMachineContext* context));
    MOCK_METHOD1_T(handleEvent2, void (StateMachineContext* context));
    MOCK_METHOD1_T(handleEvent3, void (StateMachineContext* context));
    MOCK_METHOD1_T(handleEvent4, void (StateMachineContext* context));

	// IStateMachineHooks
	MOCK_METHOD1(initializeImpl, bool (bool force));
	MOCK_METHOD1(finalizeImpl,void (bool finalizeSubStateMachines));
    MOCK_METHOD0(subStateMachineCompleted, void ());
    MOCK_METHOD0(subStateMachineCompletedImpl, void ());
    MOCK_CONST_METHOD0_T(getInitialStateImpl, InnerStateClass* ());
    MOCK_CONST_METHOD0(isReadyImpl, bool ());

    // IStateHooks
    MOCK_METHOD1_T(entryImpl, void (StateMachineContext* context));
    MOCK_METHOD1_T(exitImpl, void (StateMachineContext* context));
    MOCK_METHOD1_T(startDoImpl, void (StateMachineContext* context));
    MOCK_METHOD1_T(endDoImpl, void (StateMachineContext* context));
    MOCK_METHOD1(finalizeSubStateMachinesImpl, void (bool recursive));
    MOCK_METHOD1(initSubStateMachinesImpl, void (bool recursive));
    MOCK_METHOD3_T(checkDirectTransitionImpl, bool (StateMachineContext* context, bool& finalize, sttcl::StateBase<StateMachineContext,ITestStateInterface>*& nextState));
    MOCK_METHOD2_T(doActionImpl, void (StateMachineContext* context, bool firstCall));
    MOCK_METHOD1_T(joinDoActionImpl, void (StateMachineContext* context));


protected:
	mutable sttcl::internal::SttclMutex<> internalGuard_;
	InnerStateClass* initialState_;
    bool enableLogs_;
    std::string stateMachineId_;
    typename StateBaseClass::StateBaseType* directTransitState_;
    bool finalizeOnNextDirectTransitCheck_;

private:
	bool isReadyImplCall() const
    {
		if(logsEnabled())
		{
			STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::isReadyImpl() ...");
		}
    	return SttclCompositeStateMock::StateMachineBaseClass::isReadyImpl();
    }

    bool initializeImplCall(bool force)
    {
        if(logsEnabled())
		{
			STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::initializeImpl(" << force << ") ...");
		}
    	return StateMachineBaseClass::initializeImpl(force);
    }

    void finalizeImplCall(bool finalizeSubStateMachines)
    {
        if(logsEnabled())
		{
			STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::finalizeImpl(" << finalizeSubStateMachines << ") ...");
		}
		StateMachineBaseClass::finalizeImpl(finalizeSubStateMachines);
    }

    void subStateMachineCompletedCall()
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::subStateMachineCompleted() ...");
        }
        StateMachineBaseClass::subStateMachineCompleted();
    }

    void subStateMachineCompletedImplCall0()
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::subStateMachineCompletedImpl() ...");
        }
        StateMachineBaseClass::subStateMachineCompletedImpl();
    }

    void subStateMachineCompletedImplCall1(IStateMachineHooks<ITestInnerStateInterface>::StateBaseClass* state)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateMachineBaseClass::subStateMachineCompletedImpl() ...");
        }
        StateMachineBaseClass::subStateMachineCompleted();
    }

    InnerStateClass* getInitialStateImplCall() const
    {
        if(logsEnabled())
		{
			STTCL_TEST_LOG(id() << " SttclCompositeStateMock::getInitialStateImplCall(), initialState: = " << initialState_);
		}
    	sttcl::internal::AutoLocker<sttcl::internal::SttclMutex<> > lock(internalGuard_);
    	return initialState_;
    }


    void entryImplCall(StateMachineContext* context)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::entryImpl( context = " << context << ") ...");
        }
        StateMachineBaseClass::entryImpl(context);
    }

    void exitImplCall(StateMachineContext* context)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::exitImpl( context = " << context << ") ...");
        }
        StateMachineBaseClass::exitImpl(context);
    }

    void startDoImplCall(StateMachineContext* context)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::startDoImpl( context = " << context << ") ...");
        }
        StateBaseClass::startDoImpl(context);
    }

    void endDoImplCall(StateMachineContext* context)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::endDoImpl( context = " << context << ") ...");
        }
        StateBaseClass::endDoImpl(context);
    }

    void finalizeSubStateMachinesImplCall(bool recursive)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::finalizeSubStateMachinesImpl(" << recursive << ") ...");
        }
        StateBaseClass::finalizeSubStateMachinesImpl(recursive);
    }

    void initSubStateMachinesImplCall(bool recursive)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::initSubStateMachinesImpl(" << recursive << ") ...");
        }
        StateBaseClass::initSubStateMachinesImpl(recursive);
    }

    bool checkDirectTransitionImplCall(StateMachineContext* context, bool& finalize, sttcl::StateBase<StateMachineContext,ITestStateInterface>*& nextState)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::checkDirectTransitionImpl( context = " << context << ", finalize = " << finalize << ", nextState = " << nextState << ") ...");
        }
        sttcl::internal::AutoLocker<sttcl::internal::SttclMutex<> > lock(internalGuard_);
        if(!StateBaseClass::checkDirectTransitionImpl(context,finalize,nextState))
        {
            if(directTransitState_)
            {
                nextState = directTransitState_;
                return true;
            }
            else if(finalizeOnNextDirectTransitCheck_)
            {
                finalizeOnNextDirectTransitCheck_ = false;
                nextState = NULL;
                finalize = true;
                return true;
            }

        }
        else
        {
            return true;
        }
        return false;
    }

    void doActionImplCall(StateMachineContext* context, bool firstCall)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " SttclCompositeStateMock::doActionImplCall( context = " << context << ", firstCall = " << firstCall);
        }
    }

    void joinDoActionImplCall(StateMachineContext* context)
    {
        if(logsEnabled())
        {
            STTCL_TEST_LOG(id() << " Calling SttclCompositeStateMock::StateBaseClass::joinDoActionImpl(" << context << ") ...");
        }
        StateBaseClass::joinDoActionImpl(context);
    }

};
#endif /* STTCLCOMPOSITESTATEMOCK_H_ */
