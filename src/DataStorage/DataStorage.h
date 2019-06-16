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
    Data* _dataArray[PUMP_AMOUNT];
public:
    DataStorage(int amount);
    bool GetIsReady(int index);
    void SaveDataIfNeeded(int index, Data data);
    void Init();
    Data* GetData(int index);
};

#endif
