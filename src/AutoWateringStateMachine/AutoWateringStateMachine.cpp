#include "AutoWateringStateMachine.h"

void AutoWateringStateMachine::Run(Command command)
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
AutoWateringState AutoWateringStateMachine::GetState()
{
    return _autoWateringState;
}
void AutoWateringStateMachine::HandleEncoderClickIfNeeded()
{
    switch (_autoWateringState)
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
void AutoWateringStateMachine::HandleEncoderLeftTurnIfNeeded()
{
    MovePreviousState();
}
void AutoWateringStateMachine::HandleEncoderRightTurnIfNeeded()
{
    MoveNextState();
}

bool AutoWateringStateMachine::IsChangableValueState(){
 switch (_autoWateringState)
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
void AutoWateringStateMachine::HandleEncoderHoldLeftTurnIfNeeded()
{
   if(!IsChangableValueState() || !_onDecreaseValueCallback)
    return;

    _onDecreaseValueCallback();
}
void AutoWateringStateMachine::HandleEncoderHoldRightTurnIfNeeded()
{
     if(!IsChangableValueState() || !_onIncreaseValueCallback)
    return;

    _onIncreaseValueCallback();
}
void AutoWateringStateMachine::SetState(AutoWateringState newState)
{
    if(_autoWateringState == newState)
        return;

    auto oldState = _autoWateringState;
    _autoWateringState = newState;

    if (_onStateChangedCallback)
        _onStateChangedCallback();
}
void AutoWateringStateMachine::MoveNextState()
{
    SetState(GetNextState());
}
void AutoWateringStateMachine::MovePreviousState()
{
    SetState(GetPreviousState());
}
AutoWateringState AutoWateringStateMachine::GetNextState()
{
    switch (_autoWateringState)
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
AutoWateringState AutoWateringStateMachine::GetPreviousState()
{
    switch (_autoWateringState)
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

void AutoWateringStateMachine::AttachOnStateChanged(autoWateringStateMachineCallback callback)
{
    _onStateChangedCallback = callback;
}
void AutoWateringStateMachine::AttachOnDecreaseValue(autoWateringStateMachineCallback callback)
{
    _onDecreaseValueCallback = callback;
}
void AutoWateringStateMachine::AttachOnIncreaseValue(autoWateringStateMachineCallback callback)
{
    _onIncreaseValueCallback = callback;
}
void AutoWateringStateMachine::AttachOnLeftSettings(autoWateringStateMachineCallback callback)
{
    _onLeftSettingsCallback = callback;
}
void AutoWateringStateMachine::AttachOnBeforeEnterToSettings(autoWateringStateMachineCallback callback)
{
    _onBeforeEnterToSettingsCallback = callback;
}
