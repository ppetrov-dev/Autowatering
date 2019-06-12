#ifndef dataStorageH
#define dataStorageH

#include <EEPROM.h>
#include <Arduino.h>
#include "Data.h"
#include "settings.h"

class DataStorage
{

private:
    unsigned int _eepromAddress = 130;
    int _amount;
    Data *_dataArray[PUPM_AMOUNT];
    unsigned long _timestampArray[PUPM_AMOUNT];
    unsigned int GetDataAddress();
    unsigned int GetTimestampAddress();

public:
    DataStorage(int amount);
    bool GetIsReady(int index);
    void SaveDataIfNeeded(int index, Data data);
    void SaveTimestampIfNeeded(int index, unsigned long timestamp);
    void Init();
    Data* GetData(int index);
    unsigned long GetTimestamp(int index);
};

#endif
