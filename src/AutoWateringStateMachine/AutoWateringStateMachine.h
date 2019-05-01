#ifndef autoWateringStateMachineH
#define autoWateringStateMachineH

#include "enums.h"

extern "C" {
    typedef void (*autoWateringStateMachineCallback)(void);
}

class AutoWateringStateMachine
{
private:
    autoWateringStateMachineCallback _onStateChangedCallback;
    autoWateringStateMachineCallback _onDecreaseValueCallback;
    autoWateringStateMachineCallback _onIncreaseValueCallback;
    autoWateringStateMachineCallback _onLeftSettingsCallback;
    autoWateringStateMachineCallback _onBeforeEnterToSettingsCallback;

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

    void AttachOnStateChanged(autoWateringStateMachineCallback callback);
    void AttachOnDecreaseValue(autoWateringStateMachineCallback callback);
    void AttachOnIncreaseValue(autoWateringStateMachineCallback callback);
    void AttachOnLeftSettings(autoWateringStateMachineCallback callback);
    void AttachOnBeforeEnterToSettings(autoWateringStateMachineCallback callback);
};


#endif //autoWateringStateMachineH