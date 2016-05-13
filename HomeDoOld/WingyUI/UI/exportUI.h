#ifdef WINGY_UI_DLL
#define UI_API  __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif