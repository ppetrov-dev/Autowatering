#ifndef autoWateringEncoderH
#define autoWateringEncoderH
#include "GyverEncoder.h"
extern "C"
{
    typedef void (*callbackFunction)(void);
}

class AutoWateringEncoder
{
private:
    Encoder _encoder;
    callbackFunction _onRightTurnCallbackFunc = NULL;
    callbackFunction _onLeftTurnCallbackFunc = NULL;
    callbackFunction _onRightHoldTurnCallbackFunc = NULL;
    callbackFunction _onLeftHoldTurnCallbackFunc = NULL;
    callbackFunction _onClickCallbackFunc = NULL;
public:
    AutoWateringEncoder(byte pinClk, byte pinDt, byte pinSw);
    bool IsHold();
    void Tick(void);
    void AttachOnClick(callbackFunction newFunction);
    void AttachOnRightTurn(callbackFunction newFunction);
    void AttachOnLeftTurn(callbackFunction newFunction);
    void AttachOnRightHoldTurn(callbackFunction newFunction);
    void AttachOnLeftHoldTurn(callbackFunction newFunction);
    void SetEncoderType(bool isDefaultEncoderType = false);
    void SetEncoderDirection(bool isReversedEncoderDirection = false);
};
#endif