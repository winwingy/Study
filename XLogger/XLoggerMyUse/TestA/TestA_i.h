

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue May 03 17:16:58 2016
 */
/* Compiler settings for TestA.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __TestA_i_h__
#define __TestA_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IHolleWorld_FWD_DEFINED__
#define __IHolleWorld_FWD_DEFINED__
typedef interface IHolleWorld IHolleWorld;

#endif 	/* __IHolleWorld_FWD_DEFINED__ */


#ifndef __HolleWorld_FWD_DEFINED__
#define __HolleWorld_FWD_DEFINED__

#ifdef __cplusplus
typedef class HolleWorld HolleWorld;
#else
typedef struct HolleWorld HolleWorld;
#endif /* __cplusplus */

#endif 	/* __HolleWorld_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IHolleWorld_INTERFACE_DEFINED__
#define __IHolleWorld_INTERFACE_DEFINED__

/* interface IHolleWorld */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHolleWorld;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8EE9CD15-85A0-45B1-892F-463A8AB1D8D0")
    IHolleWorld : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IHolleWorldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHolleWorld * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHolleWorld * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHolleWorld * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHolleWorld * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHolleWorld * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHolleWorld * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHolleWorld * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IHolleWorldVtbl;

    interface IHolleWorld
    {
        CONST_VTBL struct IHolleWorldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHolleWorld_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHolleWorld_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHolleWorld_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHolleWorld_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHolleWorld_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHolleWorld_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHolleWorld_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHolleWorld_INTERFACE_DEFINED__ */



#ifndef __TestALib_LIBRARY_DEFINED__
#define __TestALib_LIBRARY_DEFINED__

/* library TestALib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_TestALib;

EXTERN_C const CLSID CLSID_HolleWorld;

#ifdef __cplusplus

class DECLSPEC_UUID("059BB5AC-D4C3-4A83-852B-0559D178C46E")
HolleWorld;
#endif
#endif /* __TestALib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


