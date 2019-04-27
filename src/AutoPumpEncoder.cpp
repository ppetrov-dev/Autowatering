#include "AutoPumpEncoder.h"

AutoPumpEncoder::AutoPumpEncoder(byte pinClk, byte pinDt, byte pinSw)
    : _encoder(pinClk, pinDt, pinSw)
{
}

void AutoPumpEncoder::SetEncoderType(bool isDefaultEncoderType = false)
{
    _encoder.setType(isDefaultEncoderType);
}
void AutoPumpEncoder::SetEncoderDirection(bool isReversedEncoderDirection = false)
{
    _encoder.setDirection(isReversedEncoderDirection);
}

void AutoPumpEncoder::AttachOnAnyTurn(callbackFunction newFunction)
{
    _onAnyTurnCallbackFunc = newFunction;
}

void AutoPumpEncoder::AttachOnLeftTurn(callbackFunction newFunction)
{
    _onLeftTurnCallbackFunc = newFunction;
}

void AutoPumpEncoder::AttachOnRightTurn(callbackFunction newFunction)
{
    _onRightTurnCallbackFunc = newFunction;
}

void AutoPumpEncoder::AttachOnLeftHoldTurn(callbackFunction newFunction)
{
    _onLeftHoldTurnCallbackFunc = newFunction;
}

void AutoPumpEncoder::AttachOnRightHoldTurn(callbackFunction newFunction)
{
    _onRightHoldTurnCallbackFunc = newFunction;
}
void AutoPumpEncoder::AttachOnClick(callbackFunction newFunction)
{
    _onClickCallbackFunc = newFunction;
}
bool AutoPumpEncoder::IsHold()
{
    return _encoder.isPress();
}
void AutoPumpEncoder::Tick()
{
    _encoder.tick();

    if (_encoder.isTurn() && _onAnyTurnCallbackFunc != NULL)
        _onAnyTurnCallbackFunc();

    if (_encoder.isRight() && _onRightTurnCallbackFunc != NULL)
        _onRightTurnCallbackFunc();

    if (_encoder.isLeft() && _onLeftTurnCallbackFunc != NULL)
        _onLeftTurnCallbackFunc();

    if (_encoder.isRightH() && _onRightHoldTurnCallbackFunc != NULL)
        _onRightHoldTurnCallbackFunc();

    if (_encoder.isLeftH() && _onLeftHoldTurnCallbackFunc != NULL)
        _onLeftHoldTurnCallbackFunc();
    
    if(_encoder.isClick() && _onClickCallbackFunc!= NULL)
        _onClickCallbackFunc();
}
