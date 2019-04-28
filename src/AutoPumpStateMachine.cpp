#include "AutoPumpStateMachine.h"

AutoPumpState AutoPumpStateMachine::GetState(){
    return _autoPumpState;
}

void AutoPumpStateMachine::Run(Command command)
{
    switch (command)
    {
    case EncoderLeftTurnCommand:
        HandleEncoderLeftTurnIfNeeded();
        break;
    case EncoderRightTurnCommand:
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

void AutoPumpStateMachine::HandleEncoderClickIfNeeded()
{
    switch (_autoPumpState)
    {
    case SelectSettingsState:
        SetState(SelectBackState);
        if(_onLeftSettingsCallback)
            _onLeftSettingsCallback();
        break;
    case SelectBackState:
        SetState(SelectSettingsState);
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
    case SelectPauseDaysState:
    case SelectPauseHoursState:
    case SelectPauseMinutesState:
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
        return SelectPauseDaysState;
    case SelectPauseDaysState:
        return SelectPauseHoursState;
    case SelectPauseHoursState:
        return SelectPauseMinutesState;
    case SelectPauseMinutesState:
        return SelectWorkHoursState;
    case SelectWorkHoursState:
        return SelectWorkMinutesState;
    case SelectWorkMinutesState:
        return SelectWorkSecondsState;
    case SelectWorkSecondsState:
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
        return SelectWorkSecondsState;
    case SelectPauseDaysState:
        return SelectBackState;
    case SelectPauseHoursState:
        return SelectPauseDaysState;
    case SelectPauseMinutesState:
        return SelectPauseHoursState;
    case SelectWorkHoursState:
        return SelectPauseMinutesState;
    case SelectWorkMinutesState:
        return SelectWorkHoursState;
    case SelectWorkSecondsState:
        return SelectWorkMinutesState;
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
