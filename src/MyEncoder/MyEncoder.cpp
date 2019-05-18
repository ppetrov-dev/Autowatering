#include "MyEncoder.h"

MyEncoder::MyEncoder(byte pinClk, byte pinDt, byte pinSw)
    : _encoder(pinClk, pinDt, pinSw)
{
}

void MyEncoder::SetEncoderType(bool isDefaultEncoderType = false)
{
    _encoder.setType(isDefaultEncoderType);
}
void MyEncoder::SetEncoderDirection(bool isReversedEncoderDirection = false)
{
    _encoder.setDirection(isReversedEncoderDirection);
}

void MyEncoder::AttachOnLeftTurn(callbackFunction newFunction)
{
    _onLeftTurnCallbackFunc = newFunction;
}

void MyEncoder::AttachOnRightTurn(callbackFunction newFunction)
{
    _onRightTurnCallbackFunc = newFunction;
}

void MyEncoder::AttachOnLeftHoldTurn(callbackFunction newFunction)
{
    _onLeftHoldTurnCallbackFunc = newFunction;
}

void MyEncoder::AttachOnRightHoldTurn(callbackFunction newFunction)
{
    _onRightHoldTurnCallbackFunc = newFunction;
}
void MyEncoder::AttachOnClick(callbackFunction newFunction)
{
    _onClickCallbackFunc = newFunction;
}
bool MyEncoder::IsHold()
{
    return _encoder.isPress();
}
void MyEncoder::Tick()
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
