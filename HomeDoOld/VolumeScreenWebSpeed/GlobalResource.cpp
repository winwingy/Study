#include "StdAfx.h"
#include "GlobalResource.h"

GlobalResource* GlobalResource::global_ = NULL;
GlobalResource::GlobalResource(void)
    : MainHwnd_(NULL)
{
}


GlobalResource::~GlobalResource(void)
{
}


GlobalResource* GlobalResource::GetInstance()
{
    if (global_ == NULL)
    {
        global_ = new GlobalResource;
    }
    return global_;
}