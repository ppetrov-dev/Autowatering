#ifndef autoWateringStateMachineH
#define autoWateringStateMachineH

#include "enums.h"

extern "C" {
    typedef void (*stateMachineCallback)(void);
}

class AutoWateringStateMachine
{
private:
    stateMachineCallback _onStateChangedCallback;
    stateMachineCallback _onDecreaseValueCallback;
    stateMachineCallback _onIncreaseValueCallback;
    stateMachineCallback _onLeftSettingsCallback;
    stateMachineCallback _onBeforeEnterToSettingsCallback;

    enum AutoWateringState _autoWateringState = SelectPumpState; 

    void HandleEncoderClickIfNeeded(void);
    void HandleEncoderHoldLeftTurnIfNeeded(void);
    void HandleEncoderHoldRightTurnIfNeeded(void);
    void HandleEncoderLeftTurnIfNeeded(void);
    void HandleEncoderRightTurnIfNeeded(void);

    AutoWateringState GetNextState();
    AutoWateringState GetPreviousState();
    void MoveNextState(void);
    void MovePreviousState(void);
    bool IsChangableValueState(void);
    void SetState(AutoWateringState newState);
    
public:
    void Run(Command command);
    AutoWateringState GetState();

    void AttachOnStateChanged(stateMachineCallback callback);
    void AttachOnDecreaseValue(stateMachineCallback callback);
    void AttachOnIncreaseValue(stateMachineCallback callback);
    void AttachOnLeftSettings(stateMachineCallback callback);
    void AttachOnBeforeEnterToSettings(stateMachineCallback callback);
};


#endif //autoWateringStateMachineH