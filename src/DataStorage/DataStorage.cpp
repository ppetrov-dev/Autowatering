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

bool DataStorage::EqualsData(Data* savedData, Data* data){
    return savedData->WaitTimeInMinutes == data->WaitTimeInMinutes && savedData->WorkTimeInSeconds == data->WorkTimeInSeconds
    && savedData->LastWateringTimeStampInSeconds == data->LastWateringTimeStampInSeconds;
} 

bool DataStorage::GetIsReady(int index)
{
    auto data = _dataArray[index];
    return data->WaitTimeInMinutes != ULONG_MAX && data->WorkTimeInSeconds != ULONG_MAX && data->LastWateringTimeStampInSeconds != ULONG_MAX;
}

void DataStorage::Init()
{
    auto dataAddress = GetDataAddress();
    Data *data = NULL;
    for (int i = 0; i < _amount; i++,
             dataAddress += sizeof(Data))
    {
        data = new Data();
        EEPROM.get(dataAddress, *data);
        _dataArray[i] = data;
    }
}

Data *DataStorage::GetData(int index)
{
    auto data = _dataArray[index];
    Serial.println("DataStorage|GetData| index: " + String(index) + " waitTimeInMinutes: " + String(data->WaitTimeInMinutes) + " workTimeInSeconds: " + String(data->WorkTimeInSeconds)+ " timeStampInSeconds: " + String(data->LastWateringTimeStampInSeconds));
    return data;
}

void DataStorage::SaveDataIfNeeded(int index, Data data)
{
    auto address = GetDataAddress() + sizeof(Data) * index;
    auto savedData = GetData(index);

    if(EqualsData(savedData, &data))
        return;

    EEPROM.put(address, data);

    savedData->WaitTimeInMinutes = data.WaitTimeInMinutes;
    savedData->WorkTimeInSeconds = data.WorkTimeInSeconds;
    savedData->LastWateringTimeStampInSeconds = data.LastWateringTimeStampInSeconds;
 
    Serial.println("DataStorage|SaveDataIfNeeded| index: " + String(index) + " waitTimeInMinutes: " + String(savedData->WaitTimeInMinutes) + " workTimeInSeconds: " + String(savedData->WorkTimeInSeconds)+ " timeStampInSeconds: " + String(savedData->LastWateringTimeStampInSeconds));
}

void DataStorage::SaveDataIfNeeded(int index, unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds, unsigned long timeStampInSeconds){
    Data data(waitTimeInMinutes, workTimeInSeconds, timeStampInSeconds);
    SaveDataIfNeeded(index, data);
}

void DataStorage::SaveDataIfNeeded(int index, unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
{
  auto savedData = GetData(index);
  SaveDataIfNeeded(index, waitTimeInMinutes, workTimeInSeconds,savedData->LastWateringTimeStampInSeconds);
}

void DataStorage::SaveDataIfNeeded(int index, unsigned long timeStampInSeconds)
{
  auto savedData = GetData(index);
  SaveDataIfNeeded(index, savedData->WaitTimeInMinutes, savedData->WorkTimeInSeconds, timeStampInSeconds);
}