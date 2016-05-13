#pragma once
class AnimatorTool
{
public:
    static AnimatorTool* GetInstance();
    static void ReleaseInstance();

private:
    AnimatorTool(void);
    ~AnimatorTool(void);
    static AnimatorTool* animatorTool_;
};

