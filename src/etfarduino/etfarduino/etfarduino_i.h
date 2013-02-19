

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Feb 19 01:14:08 2013
 */
/* Compiler settings for .\etfarduino.idl:
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

#ifndef __etfarduino_i_h__
#define __etfarduino_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IetfarduinoAin_FWD_DEFINED__
#define __IetfarduinoAin_FWD_DEFINED__
typedef interface IetfarduinoAin IetfarduinoAin;
#endif 	/* __IetfarduinoAin_FWD_DEFINED__ */


#ifndef __IetfarduinoAout_FWD_DEFINED__
#define __IetfarduinoAout_FWD_DEFINED__
typedef interface IetfarduinoAout IetfarduinoAout;
#endif 	/* __IetfarduinoAout_FWD_DEFINED__ */


#ifndef __IetfarduinoDio_FWD_DEFINED__
#define __IetfarduinoDio_FWD_DEFINED__
typedef interface IetfarduinoDio IetfarduinoDio;
#endif 	/* __IetfarduinoDio_FWD_DEFINED__ */


#ifndef __etfarduinoadapt_FWD_DEFINED__
#define __etfarduinoadapt_FWD_DEFINED__

#ifdef __cplusplus
typedef class etfarduinoadapt etfarduinoadapt;
#else
typedef struct etfarduinoadapt etfarduinoadapt;
#endif /* __cplusplus */

#endif 	/* __etfarduinoadapt_FWD_DEFINED__ */


#ifndef __etfarduinoAin_FWD_DEFINED__
#define __etfarduinoAin_FWD_DEFINED__

#ifdef __cplusplus
typedef class etfarduinoAin etfarduinoAin;
#else
typedef struct etfarduinoAin etfarduinoAin;
#endif /* __cplusplus */

#endif 	/* __etfarduinoAin_FWD_DEFINED__ */


#ifndef __etfarduinoAout_FWD_DEFINED__
#define __etfarduinoAout_FWD_DEFINED__

#ifdef __cplusplus
typedef class etfarduinoAout etfarduinoAout;
#else
typedef struct etfarduinoAout etfarduinoAout;
#endif /* __cplusplus */

#endif 	/* __etfarduinoAout_FWD_DEFINED__ */


#ifndef __etfarduinoDio_FWD_DEFINED__
#define __etfarduinoDio_FWD_DEFINED__

#ifdef __cplusplus
typedef class etfarduinoDio etfarduinoDio;
#else
typedef struct etfarduinoDio etfarduinoDio;
#endif /* __cplusplus */

#endif 	/* __etfarduinoDio_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "daqmex.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IetfarduinoAin_INTERFACE_DEFINED__
#define __IetfarduinoAin_INTERFACE_DEFINED__

/* interface IetfarduinoAin */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IetfarduinoAin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A69C8730-50F7-46ab-A011-C32A9623A9B2")
    IetfarduinoAin : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IetfarduinoAinVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IetfarduinoAin * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IetfarduinoAin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IetfarduinoAin * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IetfarduinoAin * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IetfarduinoAin * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IetfarduinoAin * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IetfarduinoAin * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IetfarduinoAinVtbl;

    interface IetfarduinoAin
    {
        CONST_VTBL struct IetfarduinoAinVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IetfarduinoAin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IetfarduinoAin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IetfarduinoAin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IetfarduinoAin_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IetfarduinoAin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IetfarduinoAin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IetfarduinoAin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IetfarduinoAin_INTERFACE_DEFINED__ */


#ifndef __IetfarduinoAout_INTERFACE_DEFINED__
#define __IetfarduinoAout_INTERFACE_DEFINED__

/* interface IetfarduinoAout */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IetfarduinoAout;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("26EFD51E-8AC5-4645-8FC8-CECF6E29A710")
    IetfarduinoAout : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IetfarduinoAoutVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IetfarduinoAout * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IetfarduinoAout * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IetfarduinoAout * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IetfarduinoAout * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IetfarduinoAout * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IetfarduinoAout * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IetfarduinoAout * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IetfarduinoAoutVtbl;

    interface IetfarduinoAout
    {
        CONST_VTBL struct IetfarduinoAoutVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IetfarduinoAout_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IetfarduinoAout_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IetfarduinoAout_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IetfarduinoAout_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IetfarduinoAout_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IetfarduinoAout_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IetfarduinoAout_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IetfarduinoAout_INTERFACE_DEFINED__ */


#ifndef __IetfarduinoDio_INTERFACE_DEFINED__
#define __IetfarduinoDio_INTERFACE_DEFINED__

/* interface IetfarduinoDio */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IetfarduinoDio;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B7100710-F719-4c12-B847-ABC147B9F847")
    IetfarduinoDio : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IetfarduinoDioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IetfarduinoDio * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IetfarduinoDio * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IetfarduinoDio * This);
        
        END_INTERFACE
    } IetfarduinoDioVtbl;

    interface IetfarduinoDio
    {
        CONST_VTBL struct IetfarduinoDioVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IetfarduinoDio_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IetfarduinoDio_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IetfarduinoDio_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IetfarduinoDio_INTERFACE_DEFINED__ */



#ifndef __etfarduinoLib_LIBRARY_DEFINED__
#define __etfarduinoLib_LIBRARY_DEFINED__

/* library etfarduinoLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_etfarduinoLib;

EXTERN_C const CLSID CLSID_etfarduinoadapt;

#ifdef __cplusplus

class DECLSPEC_UUID("013D390C-EA84-4d05-907F-15B5903A2952")
etfarduinoadapt;
#endif

EXTERN_C const CLSID CLSID_etfarduinoAin;

#ifdef __cplusplus

class DECLSPEC_UUID("DF13DDD3-AB25-4470-A5CA-0CFCE6A4E1C5")
etfarduinoAin;
#endif

EXTERN_C const CLSID CLSID_etfarduinoAout;

#ifdef __cplusplus

class DECLSPEC_UUID("F5D02ED5-BAA4-49f1-AE9B-F63F0E82A1C5")
etfarduinoAout;
#endif

EXTERN_C const CLSID CLSID_etfarduinoDio;

#ifdef __cplusplus

class DECLSPEC_UUID("5EF1299B-0EA2-4039-AB34-012FD8F978F2")
etfarduinoDio;
#endif
#endif /* __etfarduinoLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


