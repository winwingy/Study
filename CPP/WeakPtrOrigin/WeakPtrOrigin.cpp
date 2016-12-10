// WeakPtrOrigin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <memory>
#include <string>

class Teacher
{
public:
    Teacher(const std::wstring& name)
        : name_(name)
    {

    }
    ~Teacher()
    {

    }



private:
    std::wstring name_;
};

struct PointIsMember
{
    int (*Add) (int a, int b);
};

int AddDouble(int a, int b)
{
    return a + 2 * 2;
}


int _tmain(int argc, _TCHAR* argv[])
{
    {
        PointIsMember pim = { AddDouble };
        if (pim.Add)
        {
            int c = pim.Add(1, 2);
            int b = 5;
        }

    }

    {
        std::shared_ptr<Teacher> shTeacher(new Teacher(L"ABC"));
        std::weak_ptr<Teacher> weakTeacher(shTeacher);
        shTeacher.reset();
        auto aaa = weakTeacher.lock();

        int a = 1;
    }

	return 0;
}

