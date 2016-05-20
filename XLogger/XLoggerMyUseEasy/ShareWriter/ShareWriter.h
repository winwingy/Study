#pragma once
#include <tchar.h>
#include <string>
#include <windows.h>
#include <vector>

#define MAX_ITEM_COUNT 1500

#ifdef _UNICODE
#define FUNCTIONNAME __FUNCTIONW__
#define tstring std::wstring
#define tstringstream std::wstringstream
#else
#define FUNCTIONNAME __FUNCTION__
#define tstring std::string
#define tstringstream std::stringstream
#endif

struct MemItemData;
#define ItemBuffSize 1024 * 5

class ShareWriter
{
public:
    ShareWriter();
    ~ShareWriter();

    void Create(const tstring& exePath);
    bool Write(const TCHAR* text, int len);
    bool Read(TCHAR* buf, int bufLen, int* getted);

private:
    bool WriteToInternal(const TCHAR* text, int len, int* free);
    void SleepTime(int free);
    void close();
    int* readIndex_;
    int* writeIndex_;
    void* dataIndex_;
    HANDLE fileMapping_;
    void* mapView_;
    MemItemData* memDataList_[MAX_ITEM_COUNT];
    HANDLE writeMutex_;
    HANDLE readEvent_;

};

