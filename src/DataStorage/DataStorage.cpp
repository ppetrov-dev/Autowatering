#include "DataStorage.h"

DataStorage::DataStorage(int amount)
{
    _amount = amount;
    for (int i = 0; i < amount; i++)
        _dataArray[i] = new Data();
}

void DataStorage::SaveDataIfNeeded(int index, Data data)
{
    auto address = _eepromAddress;
    auto savedData = _dataArray[index];

    if (savedData->WaitTimeInMinutes == data.WaitTimeInMinutes && savedData->WorkTimeInSeconds == data.WorkTimeInSeconds)
        return;

    for (int i = 0; i < _amount; i++)
    {
        if (index == i)
        {
            EEPROM.put(address, data);
            break;
        }
        address += sizeof(Data);
    }

    savedData->WaitTimeInMinutes = data.WaitTimeInMinutes;
    savedData->WorkTimeInSeconds = data.WorkTimeInSeconds;
}

bool DataStorage::GetIsReady(int index)
{
    return _dataArray[index]->WaitTimeInMinutes != ULONG_MAX && _dataArray[index]->WorkTimeInSeconds != ULONG_MAX;
}

void DataStorage::Init()
{
    auto address = _eepromAddress;
    Data *data = NULL;
    for (int i = 0; i < _amount; i++)
    {
        data = new Data();
        EEPROM.get(address, *data);
        _dataArray[i] = data;
        address += sizeof(Data);
    }
}

Data *DataStorage::GetData(int index)
{
    return _dataArray[index];
}