

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Mar 01 10:29:11 2015
 */
/* Compiler settings for Polygon.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IPolyCtrl,0xB471291B,0x3E1F,0x4CE9,0xA4,0x82,0xD2,0x22,0x00,0x58,0x21,0xFB);


MIDL_DEFINE_GUID(IID, LIBID_PolygonLib,0x522C9CF6,0x619F,0x47C6,0xA4,0x28,0x6E,0x51,0xCC,0x5B,0x6A,0x4E);


MIDL_DEFINE_GUID(IID, DIID__IPolyCtrlEvents,0xFAD52422,0xE0F7,0x47F3,0x92,0xF6,0x07,0x45,0x2A,0x8A,0xD4,0x4D);


MIDL_DEFINE_GUID(CLSID, CLSID_PolyCtrl,0xCAC853FA,0x32F8,0x49ED,0xBC,0x3F,0xA7,0x71,0x41,0xA8,0x13,0xA6);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



