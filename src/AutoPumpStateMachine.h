#ifndef autoPumpStateMachineH
#define autoPumpStateMachineH

#include "enums.h"

extern "C" {
    typedef void (*autoPumpStateMachineCallback)(void);
}

class AutoPumpStateMachine
{
private:
    autoPumpStateMachineCallback _onStateChangedCallback;
    autoPumpStateMachineCallback _onDecreaseValueCallback;
    autoPumpStateMachineCallback _onIncreaseValueCallback;
    autoPumpStateMachineCallback _onLeftSettingsCallback;

    enum AutoPumpState _autoPumpState = SelectPumpState; 

    void HandleEncoderClickIfNeeded(void);
    void HandleEncoderHoldLeftTurnIfNeeded(void);
    void HandleEncoderHoldRightTurnIfNeeded(void);
    void HandleEncoderLeftTurnIfNeeded(void);
    void HandleEncoderRightTurnIfNeeded(void);

    AutoPumpState GetNextState();
    AutoPumpState GetPreviousState();
    void MoveNextState(void);
    void MovePreviousState(void);
    bool IsChangableValueState(void);
    void SetState(AutoPumpState newState);
    
public:
    void Run(Command command);
    AutoPumpState GetState();

    void AttachOnStateChanged(autoPumpStateMachineCallback callback);
    void AttachOnDecreaseValue(autoPumpStateMachineCallback callback);
    void AttachOnIncreaseValue(autoPumpStateMachineCallback callback);
    void AttachOnLeftSettings(autoPumpStateMachineCallback callback);
};


#endif //autoPumpStateMachineH