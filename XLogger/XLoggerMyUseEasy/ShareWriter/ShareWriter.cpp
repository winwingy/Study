#include "stdafx.h"
#include "ShareWriter.h"
#include <windows.h>
#include <assert.h>



struct MemItemData
{
    int Size;
    BYTE Buf[ItemBuffSize];
};

const int HeaderLength = 50;
const int WaitTimeWrite = 1500;
const int WaitTiemRead = 100;


ShareWriter::ShareWriter()
    : readIndex_(nullptr)
    , writeIndex_(nullptr)
    , dataIndex_(nullptr)
    , fileMapping_(INVALID_HANDLE_VALUE)
    , mapView_(nullptr)
    , writeMutex_(nullptr)
    , readEvent_(nullptr)
{

}

ShareWriter::~ShareWriter()
{
    close();
}

void ShareWriter::close()
{
    if (readIndex_ && writeIndex_)
    {
        for (int i = 0; i < 10; ++i)
        {
            if (*readIndex_ == *writeIndex_)
            {
                break;
            }
            Sleep(50);
        }
        if (*readIndex_ != *writeIndex_)
        {
            assert(0 && L"*readIndex_ != *writeIndex_");
        }
    }
    if (readEvent_)
    {
        CloseHandle(readEvent_);
        readEvent_ = nullptr;
    }
    if (writeMutex_)
    {
        CloseHandle(writeMutex_);
        writeMutex_ = nullptr;
    }

    if (mapView_)
    {
        UnmapViewOfFile(mapView_);
        mapView_ = nullptr;
    }
    dataIndex_ = nullptr;
    writeIndex_ = nullptr;
    readIndex_ = nullptr;
}

tstring TranslateToEventName(const tstring& exePath)
{
    tstring eventName(exePath);
    tstring::size_type pos;
    do 
    {
        pos = eventName.find(_T("\\"));
        if (pos != tstring::npos)
        {
            eventName[pos] = _T('|');
        }
    } while (pos != tstring::npos);
    return eventName;
}

void ShareWriter::Create(const tstring& exePath)
{
    tstring eventName = TranslateToEventName(exePath);
    int totalLen = HeaderLength + sizeof(MemItemData) * MAX_ITEM_COUNT;
    fileMapping_ = CreateFileMapping(
        INVALID_HANDLE_VALUE, NULL,
        PAGE_EXECUTE_READWRITE, 0, totalLen,
        tstring(eventName + _T("ShareWriterMap")).c_str());
    assert(fileMapping_ != INVALID_HANDLE_VALUE);

    mapView_ = MapViewOfFile(fileMapping_, FILE_MAP_READ | FILE_MAP_WRITE,
                             0, 0, 0);
    assert(mapView_);
    readIndex_ = static_cast<int*>(mapView_) + 0;
    *readIndex_ = -1;
    writeIndex_ = static_cast<int*>(mapView_) + 1;
    *writeIndex_ = -1;
    dataIndex_ = reinterpret_cast<void*>(
        static_cast<BYTE*>(mapView_)+HeaderLength);
    for (int i = 0; i < MAX_ITEM_COUNT; ++i)
    {
        memDataList_[i] = reinterpret_cast<MemItemData*>(dataIndex_) + i;
    }
    //int tesst = sizeof(MemItemData) + 4;
    memDataList_[0]->Size = 0;
    writeMutex_ = CreateMutex(0, FALSE,
                              tstring(eventName + _T("ShareWriteMutex")).c_str());
    assert(writeMutex_);
    readEvent_ = CreateEvent(0, FALSE, FALSE, 
                             tstring(eventName + _T("ShareReadEvent")).c_str());
    assert(readEvent_);
}

void ShareWriter::SleepTime(int free)
{
    if (free > MAX_ITEM_COUNT/2)
    {

    }
    else if (free > MAX_ITEM_COUNT/4)
    {
        Sleep(1);
    }
    else if (free > 10)
    {
        Sleep(10);
    }
    else
    {
        Sleep(50);
    }
}

bool ShareWriter::Write(const TCHAR* text, int len)
{
    DWORD waitCode = 0;
    int free = 0;
    bool ret = false;
    for (int i = 0; i < 2; ++i)
    {
        waitCode = WaitForSingleObject(writeMutex_, WaitTimeWrite);
        if (waitCode != WAIT_TIMEOUT)
        {
            ret = WriteToInternal(text, len, &free);
            break;
        }
    }
    if (waitCode == WAIT_TIMEOUT)
    {
        assert(0 && _T("写数据等待时间太慢"));
        return false;
    }

    if (free != 0)
        SleepTime(free);

    ReleaseMutex(writeMutex_);

    return ret;
}

bool ShareWriter::WriteToInternal(const TCHAR* text, int len, int* free)
{
    if (len > ItemBuffSize)
    {
        assert(0 && L"len > ItemBuftSize");
        return false;
    }
    int writeIndex = *writeIndex_ + 1;
    if (writeIndex >= MAX_ITEM_COUNT)
    {
        writeIndex = 0;
    }

    for (int i = 0; i < WaitTimeWrite; ++i)
    {
        if (writeIndex != *readIndex_)
        {
            break;
        }
        Sleep(1);
    }

    if (writeIndex == *readIndex_)
    {
        assert(0 && L"申请空间不够， 或者读得太慢");
        return false;
    }

    memDataList_[writeIndex]->Size = len;
    memcpy(memDataList_[writeIndex]->Buf, text, len*sizeof(TCHAR));
    *free = writeIndex - *readIndex_;
    *writeIndex_ = writeIndex;
    if (*free > 0)
    {
        *free = MAX_ITEM_COUNT - *free;
    }
    else if (*free == 0)
    {
        *free = MAX_ITEM_COUNT;
    }
    else if(*free < 0)
    {
        *free = -*free;
    }
    SetEvent(readEvent_);
    return true;
}

bool ShareWriter::Read(TCHAR* buf, int bufLen, int* getted)
{
    if (bufLen < ItemBuffSize)
    {
        assert(0 && L"bufLen < ItemBuftSize");
        return false;
    }

    if (*writeIndex_ == *readIndex_)
    { 
        DWORD waitCode = WaitForSingleObject(readEvent_, WaitTiemRead);
        if (waitCode == WAIT_TIMEOUT)
        {
            return false;
        }
    }
    if (*writeIndex_ == *readIndex_)
        return false;

    ++*readIndex_;
    if (*readIndex_ >= MAX_ITEM_COUNT)
    {
        *readIndex_ = 0;
    }
    memcpy(buf, memDataList_[*readIndex_]->Buf,
           memDataList_[*readIndex_]->Size);
    *getted = memDataList_[*readIndex_]->Size;

    return true;
}
