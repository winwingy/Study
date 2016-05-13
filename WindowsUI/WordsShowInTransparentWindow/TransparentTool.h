#pragma once



class TransparentTool
{
public:
    TransparentTool(void);
    ~TransparentTool(void);
    static void RemoveBorder(HWND hWnd);
    enum ULW_VALUE
    {
        ULW_V_COLORKEY = ULW_COLORKEY,
        ULW_V_ALPHA = ULW_ALPHA,
    };
    
        //在ULW_ALPHA  下 颜色掩位无效  ULW_ALPHA（1（完全透明）－255（不透明））
        //在ULW_COLORKEY  透明度值无效
    static void TransparentAlphaWnd(HWND hWnd, bool drawSomething, 
        ULW_VALUE ulwValue, COLORREF transparentRGB, int alphaDegree); 
    static void InitGDIPlus();
   
private:
    static  HBITMAP CreateDIBBitmap(HDC hdc, int width, int height); 

};

