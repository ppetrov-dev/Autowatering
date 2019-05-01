#include "AutoWateringEncoder.h"

AutoWateringEncoder::AutoWateringEncoder(byte pinClk, byte pinDt, byte pinSw)
    : _encoder(pinClk, pinDt, pinSw)
{
}

void AutoWateringEncoder::SetEncoderType(bool isDefaultEncoderType = false)
{
    _encoder.setType(isDefaultEncoderType);
}
void AutoWateringEncoder::SetEncoderDirection(bool isReversedEncoderDirection = false)
{
    _encoder.setDirection(isReversedEncoderDirection);
}

void AutoWateringEncoder::AttachOnLeftTurn(callbackFunction newFunction)
{
    _onLeftTurnCallbackFunc = newFunction;
}

void AutoWateringEncoder::AttachOnRightTurn(callbackFunction newFunction)
{
    _onRightTurnCallbackFunc = newFunction;
}

void AutoWateringEncoder::AttachOnLeftHoldTurn(callbackFunction newFunction)
{
    _onLeftHoldTurnCallbackFunc = newFunction;
}

void AutoWateringEncoder::AttachOnRightHoldTurn(callbackFunction newFunction)
{
    _onRightHoldTurnCallbackFunc = newFunction;
}
void AutoWateringEncoder::AttachOnClick(callbackFunction newFunction)
{
    _onClickCallbackFunc = newFunction;
}
bool AutoWateringEncoder::IsHold()
{
    return _encoder.isPress();
}
void AutoWateringEncoder::Tick()
{
     _encoder.tick();

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
