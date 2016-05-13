

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __Polygon_i_h__
#define __Polygon_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPolyCtrl_FWD_DEFINED__
#define __IPolyCtrl_FWD_DEFINED__
typedef interface IPolyCtrl IPolyCtrl;
#endif 	/* __IPolyCtrl_FWD_DEFINED__ */


#ifndef ___IPolyCtrlEvents_FWD_DEFINED__
#define ___IPolyCtrlEvents_FWD_DEFINED__
typedef interface _IPolyCtrlEvents _IPolyCtrlEvents;
#endif 	/* ___IPolyCtrlEvents_FWD_DEFINED__ */


#ifndef __PolyCtrl_FWD_DEFINED__
#define __PolyCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class PolyCtrl PolyCtrl;
#else
typedef struct PolyCtrl PolyCtrl;
#endif /* __cplusplus */

#endif 	/* __PolyCtrl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IPolyCtrl_INTERFACE_DEFINED__
#define __IPolyCtrl_INTERFACE_DEFINED__

/* interface IPolyCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPolyCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B471291B-3E1F-4CE9-A482-D222005821FB")
    IPolyCtrl : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_FillColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_FillColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Sides( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Sides( 
            /* [in] */ SHORT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPolyCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPolyCtrl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPolyCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPolyCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPolyCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPolyCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPolyCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPolyCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FillColor )( 
            IPolyCtrl * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FillColor )( 
            IPolyCtrl * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Sides )( 
            IPolyCtrl * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Sides )( 
            IPolyCtrl * This,
            /* [in] */ SHORT newVal);
        
        END_INTERFACE
    } IPolyCtrlVtbl;

    interface IPolyCtrl
    {
        CONST_VTBL struct IPolyCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPolyCtrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPolyCtrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPolyCtrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPolyCtrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPolyCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPolyCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPolyCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPolyCtrl_put_FillColor(This,clr)	\
    ( (This)->lpVtbl -> put_FillColor(This,clr) ) 

#define IPolyCtrl_get_FillColor(This,pclr)	\
    ( (This)->lpVtbl -> get_FillColor(This,pclr) ) 

#define IPolyCtrl_get_Sides(This,pVal)	\
    ( (This)->lpVtbl -> get_Sides(This,pVal) ) 

#define IPolyCtrl_put_Sides(This,newVal)	\
    ( (This)->lpVtbl -> put_Sides(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPolyCtrl_INTERFACE_DEFINED__ */



#ifndef __PolygonLib_LIBRARY_DEFINED__
#define __PolygonLib_LIBRARY_DEFINED__

/* library PolygonLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PolygonLib;

#ifndef ___IPolyCtrlEvents_DISPINTERFACE_DEFINED__
#define ___IPolyCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IPolyCtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IPolyCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FAD52422-E0F7-47F3-92F6-07452A8AD44D")
    _IPolyCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IPolyCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IPolyCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IPolyCtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IPolyCtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IPolyCtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IPolyCtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IPolyCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IPolyCtrlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IPolyCtrlEventsVtbl;

    interface _IPolyCtrlEvents
    {
        CONST_VTBL struct _IPolyCtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IPolyCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IPolyCtrlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IPolyCtrlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IPolyCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IPolyCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IPolyCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IPolyCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IPolyCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PolyCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("CAC853FA-32F8-49ED-BC3F-A77141A813A6")
PolyCtrl;
#endif
#endif /* __PolygonLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


