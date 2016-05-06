#include "stdafx.h"
#include "ShareMem.h"
#include <winbase.h>
#include <assert.h>
#include <objbase.h>
#include "AutoLock.h"


const unsigned int ShareFileCount = 5;
const double ReadMustPer = 0.8;

void UnmapView(TCHAR* p)
{
    if (p)
        UnmapViewOfFile(p);
}

void CloseMapHandle(void* p)
{
    if (p != INVALID_HANDLE_VALUE &&
        p != nullptr)
        CloseHandle(p);
}

ShareMem::ShareMem()
    : memHandleA_(nullptr, CloseMapHandle)
    , memHandleB_(nullptr, CloseMapHandle)
    , mapPtrA_(nullptr, UnmapView)
    , mapPtrB_(nullptr, UnmapView)
    , write_(nullptr, UnmapView)
    , wBeg_(0)
    , wEnd_(0)
    , read_(nullptr, UnmapView)
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
    memHandleA_.reset(CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
                                   ShareFileCount * sizeof(TCHAR), L"ShareMemUseA"));
    assert(memHandleA_.get() != INVALID_HANDLE_VALUE);
    mapPtrA_.reset((TCHAR*)MapViewOfFile(memHandleA_.get(), 
                                   FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0), UnmapView);
    write_ = mapPtrA_;
    wBeg_ = 0;
    wEnd_ = 0;
    memHandleB_.reset(CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
        ShareFileCount * sizeof(TCHAR), L"ShareMemUseB"));
    assert(memHandleB_.get() != INVALID_HANDLE_VALUE);
    mapPtrB_.reset((TCHAR*)MapViewOfFile(memHandleB_.get(),
        FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0), UnmapView);
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
        write_ = mapPtrA_;
        read_ = mapPtrB_;
    }

}

bool ShareMem::Write(const TCHAR* writeText, int writeLen, int* writedLenFinish)//0 1 2 3 4 5 6
{
    int writeTextBeg = 0;
    if (writeLen <= 0)
    {
        return false;
    }
    
    AutoLock w(*dataLockWrite_);
    do
    {
        int writeWill = 0;
        do
        {
            writeWill = writeLen;// a b c
            if ((writeLen + wEnd_) > ShareFileCount)
            {
                writeWill = ShareFileCount - wEnd_;
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
                                   static_cast<float>(wEnd_) / ShareFileCount);
                    }
                    Sleep(100);
                }
                ChangeWriteReadPtr();
            }
        } while (writeWill < 0);

        memcpy(write_.get() + wEnd_, writeText + writeTextBeg,
               writeWill * sizeof(TCHAR));
        wEnd_ += writeWill;
        writeTextBeg += writeWill;
        writeLen -= writeWill;
        *writedLenFinish += writeWill;
        if (readEvent_)
            readEvent_(true, static_cast<float>(wEnd_) / ShareFileCount);

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
        readWill = bufLen;// a  b  c
        if ((rEnd_ - rBeg_) < bufLen)
        {
            readWill = rEnd_ - rBeg_;
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
        if (rEnd_ - rBeg_ <= 0)
        {
            break;
        }
    } while (readWill <= 0);

    if (0 == readWill)
    {
        return false;
    }

    memcpy(buf, read_.get() + rBeg_, readWill*sizeof(TCHAR));
    *readedLen = readWill;
    rBeg_ += readWill;
    return true;
}

void ShareMem::TryExit()
{
    while (1)
    {
        bool okW = true;
        {
            AutoLock r(*dataLockRead_);
            if (rEnd_ - rBeg_ != 0)
                okW = false;
        }
        if (!okW && readEvent_)
            readEvent_(true, 1.0);

        bool okR = true;
        {
            AutoLock w(*dataLockWrite_);
            if (wEnd_ - wBeg_ != 0)
                okR = false;
        }
        if (!okR && readEvent_)
            readEvent_(true, 1.0);

        if (okR && okW)
        {
            break;
        }

        Sleep(500);
    }
}

void ShareMem::Exit()
{

}
