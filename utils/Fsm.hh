#pragma once
#include <map>
#include <functional>

namespace UTIL {

template <typename StateType, typename InputType>
class FsmBase {
public:
    virtual void            toNextState(InputType input) = 0;
    
    virtual InputType       getNextInput() = 0;

    virtual bool            stateAcceptable() = 0;

    virtual bool            stateInvalid()  = 0;

    virtual bool            stateIgnore()  = 0;

    void                    initState(StateType s) {_state = s;}

    void                    addTransfunc(StateType state, std::function<void(InputType)>func) {_transTab[state] = func;}

protected:
    StateType               _state;

    std::map<StateType, std::function<void(InputType)>>  
                            _transTab;
};

}
