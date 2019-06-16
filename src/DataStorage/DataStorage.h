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
    unsigned int GetDataAddress();
    bool EqualsData(Data* savedData, Data* data);

    void SaveDataIfNeeded(int intex, Data data);
public:
    DataStorage(int amount);
    bool GetIsReady(int index);
    void Init();
    Data* GetData(int index);

    void SaveDataIfNeeded(int index, unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds, unsigned long timeStampInSeconds);
    void SaveDataIfNeeded(int index, unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds);
    void SaveDataIfNeeded(int index, unsigned long timeStampInSeconds);
};

#endif
