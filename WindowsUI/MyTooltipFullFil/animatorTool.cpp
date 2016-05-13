#include "StdAfx.h"
#include "animatorTool.h"

AnimatorTool* AnimatorTool::animatorTool_ = nullptr;
AnimatorTool::AnimatorTool(void)
{
}


AnimatorTool::~AnimatorTool(void)
{
}

AnimatorTool* AnimatorTool::GetInstance()
{
    if (animatorTool_ == nullptr)
    {
        animatorTool_ = new AnimatorTool();
    }
    return animatorTool_;
}

void AnimatorTool::ReleaseInstance()
{
    if (animatorTool_)
    {
        delete animatorTool_;
        animatorTool_ = nullptr;
    }
}
