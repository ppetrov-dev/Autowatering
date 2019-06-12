#include "DataStorage.h"

DataStorage::DataStorage(int amount)
{
    _amount = amount;
    for (int i = 0; i < amount; i++)
        _dataArray[i] = new Data();
}

unsigned int DataStorage::GetDataAddress()
{
    return _eepromAddress;
}

unsigned int DataStorage::GetTimestampAddress()
{
    return _eepromAddress + sizeof(Data) * _amount;
}

void DataStorage::SaveDataIfNeeded(int index, Data data)
{
    auto address = GetDataAddress() + sizeof(Data) * index;
    auto savedData = GetData(index);

    if (savedData->WaitTimeInMinutes == data.WaitTimeInMinutes && savedData->WorkTimeInSeconds == data.WorkTimeInSeconds)
        return;

    EEPROM.put(address, data);

    savedData->WaitTimeInMinutes = data.WaitTimeInMinutes;
    savedData->WorkTimeInSeconds = data.WorkTimeInSeconds;
}

void DataStorage::SaveTimestampIfNeeded(int index, unsigned long timestamp)
{
    auto address = GetTimestampAddress() + sizeof(unsigned long) * index;
    auto savedTimestamp = GetTimestamp(index);

    if (savedTimestamp == timestamp)
        return;

    EEPROM.put(address, timestamp);

    _timestampArray[index] = timestamp;
}

bool DataStorage::GetIsReady(int index)
{
    return _dataArray[index]->WaitTimeInMinutes != ULONG_MAX && _dataArray[index]->WorkTimeInSeconds != ULONG_MAX && _timestampArray[index] != ULONG_MAX;
}

void DataStorage::Init()
{
    auto dataAddress = GetDataAddress();
    auto timestampAddress = GetTimestampAddress();
    Data *data = NULL;
    long timestamp = ULONG_MAX;
    for (int i = 0; i < _amount; i++,
             dataAddress += sizeof(Data),
             timestampAddress += sizeof(unsigned long))
    {
        data = new Data();
        EEPROM.get(dataAddress, *data);
        _dataArray[i] = data;

        EEPROM.get(timestampAddress, timestamp);
        _timestampArray[i] = timestamp;
    }
}

Data *DataStorage::GetData(int index)
{
    return _dataArray[index];
}

unsigned long DataStorage::GetTimestamp(int index)
{
    return _timestampArray[index];
}