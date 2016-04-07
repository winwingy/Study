

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Dec 03 09:51:46 2013
 */
/* Compiler settings for .\XFile.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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

#ifndef __XFile_i_h__
#define __XFile_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILogFactory_FWD_DEFINED__
#define __ILogFactory_FWD_DEFINED__
typedef interface ILogFactory ILogFactory;
#endif 	/* __ILogFactory_FWD_DEFINED__ */


#ifndef __LogFactory_FWD_DEFINED__
#define __LogFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class LogFactory LogFactory;
#else
typedef struct LogFactory LogFactory;
#endif /* __cplusplus */

#endif 	/* __LogFactory_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ILogFactory_INTERFACE_DEFINED__
#define __ILogFactory_INTERFACE_DEFINED__

/* interface ILogFactory */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ILogFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1B9FF0A9-CCFC-4E4F-9051-213566E483AE")
    ILogFactory : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ LONG FileType,
            /* [in] */ BSTR AppName,
            /* [in] */ BSTR Configue,
            /* [in] */ BSTR Section,
            /* [out] */ BSTR *LogName,
            /* [out] */ BSTR *XFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogMonitor( 
            /* [in] */ LONG Handle,
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ BSTR AppName,
            /* [in] */ BSTR LogName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILogFactory * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILogFactory * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILogFactory * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILogFactory * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            ILogFactory * This,
            /* [in] */ LONG FileType,
            /* [in] */ BSTR AppName,
            /* [in] */ BSTR Configue,
            /* [in] */ BSTR Section,
            /* [out] */ BSTR *LogName,
            /* [out] */ BSTR *XFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogMonitor )( 
            ILogFactory * This,
            /* [in] */ LONG Handle,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            ILogFactory * This,
            /* [in] */ BSTR AppName,
            /* [in] */ BSTR LogName);
        
        END_INTERFACE
    } ILogFactoryVtbl;

    interface ILogFactory
    {
        CONST_VTBL struct ILogFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogFactory_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILogFactory_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILogFactory_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILogFactory_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ILogFactory_Create(This,FileType,AppName,Configue,Section,LogName,XFile)	\
    ( (This)->lpVtbl -> Create(This,FileType,AppName,Configue,Section,LogName,XFile) ) 

#define ILogFactory_LogMonitor(This,Handle,FileName)	\
    ( (This)->lpVtbl -> LogMonitor(This,Handle,FileName) ) 

#define ILogFactory_Close(This,AppName,LogName)	\
    ( (This)->lpVtbl -> Close(This,AppName,LogName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogFactory_INTERFACE_DEFINED__ */



#ifndef __XFileLib_LIBRARY_DEFINED__
#define __XFileLib_LIBRARY_DEFINED__

/* library XFileLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_XFileLib;

EXTERN_C const CLSID CLSID_LogFactory;

#ifdef __cplusplus

class DECLSPEC_UUID("742A6ED8-2816-498C-868E-C18EF812FD87")
LogFactory;
#endif
#endif /* __XFileLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


