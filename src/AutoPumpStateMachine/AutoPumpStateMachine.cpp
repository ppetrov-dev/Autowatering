#include "AutoPumpStateMachine.h"

void AutoPumpStateMachine::Run(Command command)
{
    switch (command)
    {
    case EncoderLeftTurnCommand:
        HandleEncoderLeftTurnIfNeeded();
        break;
    case EncoderRightTurnCommand:
        HandleEncoderRightTurnIfNeeded();
        break;
    case EncoderClickCommand:
        HandleEncoderClickIfNeeded();
        break;
    case EncoderHoldLeftTurnCommand:
        HandleEncoderHoldLeftTurnIfNeeded();
        break;
    case EncoderHoldRightTurnCommand:
        HandleEncoderHoldRightTurnIfNeeded();
        break;
    default:
        break;
    }
}
AutoPumpState AutoPumpStateMachine::GetState()
{
    return _autoPumpState;
}
void AutoPumpStateMachine::HandleEncoderClickIfNeeded()
{
    switch (_autoPumpState)
    {
    case SelectSettingsState:
     if(_onBeforeEnterToSettingsCallback)
            _onBeforeEnterToSettingsCallback();
        SetState(SelectBackState);
        break;
    case SelectBackState:
        SetState(SelectSettingsState);
           if(_onLeftSettingsCallback)
            _onLeftSettingsCallback();
        break;
    default:
        break;
    }
}
void AutoPumpStateMachine::HandleEncoderLeftTurnIfNeeded()
{
    MovePreviousState();
}
void AutoPumpStateMachine::HandleEncoderRightTurnIfNeeded()
{
    MoveNextState();
}

bool AutoPumpStateMachine::IsChangableValueState(){
 switch (_autoPumpState)
    {
    case SelectWaitDaysState:
    case SelectWaitHoursState:
    case SelectWaitMinutesState:
    case SelectWorkHoursState:
    case SelectWorkMinutesState:
    case SelectWorkSecondsState:
    case SelectPumpState:
        return true;
    default:
        return false;
    }
}
void AutoPumpStateMachine::HandleEncoderHoldLeftTurnIfNeeded()
{
   if(!IsChangableValueState() || !_onDecreaseValueCallback)
    return;

    _onDecreaseValueCallback();
}
void AutoPumpStateMachine::HandleEncoderHoldRightTurnIfNeeded()
{
     if(!IsChangableValueState() || !_onIncreaseValueCallback)
    return;

    _onIncreaseValueCallback();
}
void AutoPumpStateMachine::SetState(AutoPumpState newState)
{
    if(_autoPumpState == newState)
        return;

    auto oldState = _autoPumpState;
    _autoPumpState = newState;

    if (_onStateChangedCallback)
        _onStateChangedCallback();
}
void AutoPumpStateMachine::MoveNextState()
{
    SetState(GetNextState());
}
void AutoPumpStateMachine::MovePreviousState()
{
    SetState(GetPreviousState());
}
AutoPumpState AutoPumpStateMachine::GetNextState()
{
    switch (_autoPumpState)
    {
    case SelectPumpState:
        return SelectSettingsState;
    case SelectSettingsState:
        return SelectPumpState;

    case SelectBackState:
        return SelectWorkHoursState;
    case SelectWorkHoursState:
        return SelectWorkMinutesState;   
    case SelectWorkMinutesState:
        return SelectWorkSecondsState; 
    case SelectWorkSecondsState:
        return SelectWaitDaysState; 
    case SelectWaitDaysState:
        return SelectWaitHoursState;    
    case SelectWaitHoursState:
        return SelectWaitMinutesState; 
    case SelectWaitMinutesState:
        return SelectBackState;        
    }
}
AutoPumpState AutoPumpStateMachine::GetPreviousState()
{
    switch (_autoPumpState)
    {
    case SelectPumpState:
        return SelectSettingsState;
    case SelectSettingsState:
        return SelectPumpState;

    case SelectBackState:
        return SelectWaitMinutesState;
    case SelectWaitMinutesState:
        return SelectWaitHoursState;
    case SelectWaitHoursState:
        return SelectWaitDaysState;
    case SelectWaitDaysState:
        return SelectWorkSecondsState;
    case SelectWorkSecondsState:
        return SelectWorkMinutesState;
    case SelectWorkMinutesState:
        return SelectWorkHoursState;
    case SelectWorkHoursState:
        return SelectBackState;
    }
}

void AutoPumpStateMachine::AttachOnStateChanged(autoPumpStateMachineCallback callback)
{
    _onStateChangedCallback = callback;
}
void AutoPumpStateMachine::AttachOnDecreaseValue(autoPumpStateMachineCallback callback)
{
    _onDecreaseValueCallback = callback;
}
void AutoPumpStateMachine::AttachOnIncreaseValue(autoPumpStateMachineCallback callback)
{
    _onIncreaseValueCallback = callback;
}
void AutoPumpStateMachine::AttachOnLeftSettings(autoPumpStateMachineCallback callback)
{
    _onLeftSettingsCallback = callback;
}
void AutoPumpStateMachine::AttachOnBeforeEnterToSettings(autoPumpStateMachineCallback callback)
{
    _onBeforeEnterToSettingsCallback = callback;
}
