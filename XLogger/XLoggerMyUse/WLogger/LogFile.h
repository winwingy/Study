#pragma once
#include <memory>
#include <string>
enum LOG_INFO_LEVEL;

class LogFile
{
public:
    LogFile();
    ~LogFile();
    void Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
              int fileSize, int fileCount);
    void Exit();

    virtual bool WriteToFile(const TCHAR* text, int len, int* writted);

private:
    std::unique_ptr<FILE, void(*)(FILE*)> fp_;
    bool isInit_;
    std::wstring logPath_;
    int fileSize_;
    int fileCount_;
};

