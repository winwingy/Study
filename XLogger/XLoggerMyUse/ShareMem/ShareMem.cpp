#include "stdafx.h"
#include "ShareMem.h"
#include <winbase.h>
#include <assert.h>
#include <objbase.h>
#include "AutoLock.h"


const unsigned int ShareFileSize = 1024 * 1024 * 5;

ShareMem::ShareMem()
    : memHandle_(INVALID_HANDLE_VALUE)
    , write_(nullptr)
    , read_(nullptr)
    , beg_(0)
    , end_(0)
    , dataLock_(new AutoLockBase())
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
                guid.Data4[0], guid.Data4[1], guid.Data4[2])
    _snprintf

    TCHAR buf[64] = { 0 };

}

bool ShareMem::Init()
{

    memHandle_ = CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
                                   ShareFileSize, L"WLoggerShareMem");
    assert(memHandle_ != INVALID_HANDLE_VALUE);
    write_ = (TCHAR*)MapViewOfFile(memHandle_, FILE_MAP_WRITE, 0, 0, 0);
    read_ = (TCHAR*)MapViewOfFile(memHandle_, FILE_MAP_READ, 0, 0, 0);
    return true;
}

bool ShareMem::Write(const TCHAR* writeText, int writeLen, int* writedLen)
{
    AutoLock l(*dataLock_);
    if (beg_ + writeLen * sizeof(TCHAR) >= ShareFileSize)
    {
        assert(0 && L"ÉêÇëµÄÄÚ´æ²»×ã!");
        return false;
    }

    memcpy(write_ + beg_, writeText, writeLen*sizeof(TCHAR));
    end_ += writeLen*sizeof(TCHAR);
    *writedLen = writeLen*sizeof(TCHAR);
    return true;
}

bool ShareMem::Read(TCHAR* buf, int bufLen, int* readedLen)
{
    AutoLock l(*dataLock_);
    int readLen = end_ - beg_;
    if ( readLen > bufLen)
    {
        readLen = bufLen;
    }
    memcpy(buf, write_ + beg_, readLen*sizeof(TCHAR));
    *readedLen = readLen;
    beg_ += readLen;
    if (beg_ = end_)
    {
        beg_ = 0;
        end_ = 0;
    }
    else if (beg_ > end_)
    {
        assert(0 && L"beg_ > end_");
        return false;
    }
    return true;
}
