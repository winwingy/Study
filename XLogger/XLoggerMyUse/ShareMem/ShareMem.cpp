#include "stdafx.h"
#include "ShareMem.h"
#include <winbase.h>
#include <assert.h>
#include <objbase.h>
#include "AutoLock.h"


const unsigned int ShareFileSize = 1024 * 2 * sizeof(TCHAR);
const double ReadMustPer = 0.8;
const int ReadInterval = 2000; // 毫秒

ShareMem::ShareMem()
    : memHandleA_(INVALID_HANDLE_VALUE)
    , memHandleB_(INVALID_HANDLE_VALUE)
    , mapPtrA_(nullptr)
    , mapPtrB_(nullptr)
    , write_(nullptr)
    , wBeg_(0)
    , wEnd_(0)
    , read_(nullptr)
    , rBeg_(0)
    , rEnd_(0)
    , dataLockWrite_(new AutoLockBase())
    , dataLockRead_(new AutoLockBase())
{

}

ShareMem::~ShareMem()
{
}

bool GetGUIDString(TCHAR* guidStr, int bufLen)
{
    GUID guid;
    ::CoCreateGuid(&guid);

    if (bufLen < 64)
    {
        assert(0);
        return false;
    }
    _stprintf_s(guidStr, bufLen,
                _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1], guid.Data4[2],
                guid.Data4[3], guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);
    return true;
}

bool ShareMem::Init()
{
    TCHAR guiStrA[1024] = { 0 };
    bool ret = GetGUIDString(guiStrA, 1024);
    if (!ret)
    {
        return false;
    }

    TCHAR guiStrB[1024] = { 0 };
    bool retB = GetGUIDString(guiStrB, 1024);
    if (!retB)
    {
        return false;
    }
    memHandleA_ = CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
                                   ShareFileSize, L"ShareMemUse");
    assert(memHandleA_ != INVALID_HANDLE_VALUE);
    mapPtrA_ = (TCHAR*)MapViewOfFile(memHandleA_, 
                                   FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    write_ = mapPtrA_;
    wBeg_ = 0;
    wEnd_ = 0;
    memHandleB_ = CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
                                   ShareFileSize, guiStrB);
    assert(memHandleB_ != INVALID_HANDLE_VALUE);
    mapPtrB_ = (TCHAR*)MapViewOfFile(memHandleB_,
                                  FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    read_ = mapPtrB_;
    rBeg_ = 0;
    rEnd_ = 0;
    return true;
}

void ShareMem::SetReadEvent(const ReadEvent& event)
{
    readEvent_ = event;
}

void ShareMem::ChangeWriteReadPtr()
{
    AutoLock w(*dataLockWrite_);
    AutoLock r(*dataLockRead_);

    rBeg_ = wBeg_;
    rEnd_ = wEnd_;
    wBeg_ = 0;
    wEnd_ = 0;

    if (write_ == mapPtrA_)
    {
        write_ = mapPtrB_;
        read_ = mapPtrA_;
    }
    else
    {
        write_ = mapPtrB_;
        read_ = mapPtrA_;
    }

}

bool ShareMem::Write(const TCHAR* writeText, int writeLen, int* writedLen)//0 1 2 3 4 5 6
{
    int writeTextBeg = 0;
    if (writeLen <= 0)
    {
        return false;
    }
    
    AutoLock w(*dataLockWrite_);
    do
    {
        if (static_cast<float>(wEnd_ + writeLen) >=
            (ShareFileSize - 1) * ReadMustPer)
        {
            if (readEvent_)
                readEvent_(true, static_cast<float>(wEnd_) / ShareFileSize);
        }

        int writeWill = 0;
        do
        {
            writeWill = writeLen;
            if ((writeLen + wEnd_) > (ShareFileSize - 1))
            {
                writeWill = (ShareFileSize - 1) - wEnd_;
            }
            if (writeWill <= 0)
            {
                while (1)
                {
                    {
                        bool tryLock = false;
                        AutoLock r(*dataLockRead_, &tryLock);
                        if (tryLock)
                        {
                            if (0 == rBeg_ && 0 == rEnd_)
                            {
                                break;
                            }
                        }
                    } 
                    if (readEvent_)
                    {
                        readEvent_(true, 
                                   static_cast<float>(wEnd_) / ShareFileSize);
                    }
                    Sleep(100);
                }
                ChangeWriteReadPtr();
            }
        } while (writeWill < 0);

        memcpy(write_ + wEnd_, writeText + writeTextBeg, writeWill * sizeof(TCHAR));
        wEnd_ += writeWill;
        writeTextBeg += writeWill;
        writeLen -= writeWill;
        *writedLen += writeWill;
    } while (writeLen > 0);
    return true;
}

bool ShareMem::Read(TCHAR* buf, int bufLen, int* readedLen)
{
    if (bufLen <= 0)
    {
        return false;
    }

    AutoLock r(*dataLockRead_);
    int readWill = 0;
    do 
    {
        readWill = bufLen;
        if ((rEnd_ - rBeg_) < bufLen)
        {
            bufLen = rEnd_ - rBeg_;
        }
        if (readWill <= 0)
        {
            {
                bool tryLockResult = false;
                AutoLock w(*dataLockWrite_, &tryLockResult);// 写指针给占着，没必要再读了，等下次读
                if (!tryLockResult)
                {
                    return true;
                }
                ChangeWriteReadPtr();
            }
        }
    } while (readWill > 0);

    memcpy(buf, read_ + rBeg_, readWill*sizeof(TCHAR));
    *readedLen = readWill;
    rBeg_ += readWill;
    return true;
}

void ShareMem::TryExit()
{
    while (1)
    {
        {
            bool ok = true;
            {
                AutoLock r(*dataLockRead_);
                if (rEnd_ - rBeg_ != 0)
                    ok = false;
            }
            if (!ok && readEvent_)
                readEvent_(true, 1.0);
        }

        {
            bool ok = true;
            {
                AutoLock w(*dataLockWrite_);
                if (wEnd_ - wBeg_ != 0)
                    ok = false;
            }
            if (!ok && readEvent_)
                readEvent_(true, 1.0);
        }
    }
}

void ShareMem::Exit()
{

}
