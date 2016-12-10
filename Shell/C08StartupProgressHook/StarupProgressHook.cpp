// StarupProgressHook.cpp : CStarupProgressHook µÄÊµÏÖ

#include "stdafx.h"
#include "StarupProgressHook.h"


// CStarupProgressHook


HRESULT CStarupProgressHook::Execute( LPSHELLEXECUTEINFOW info )
{
    MessageBox(nullptr, L"CStarupProgressHook::Execute",
        L"CStarupProgressHook::Execute", MB_OK);
    OutputDebugString(L"CStarupProgressHook::Execute\n");
    return S_OK;
}
