/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Jan 19 21:41:44 2006
 */
/* Compiler settings for C:\sources\roms\ticables2\com\cticables2.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __cticables2_h__
#define __cticables2_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICables_FWD_DEFINED__
#define __ICables_FWD_DEFINED__
typedef interface ICables ICables;
#endif 	/* __ICables_FWD_DEFINED__ */


#ifndef __ITypes_FWD_DEFINED__
#define __ITypes_FWD_DEFINED__
typedef interface ITypes ITypes;
#endif 	/* __ITypes_FWD_DEFINED__ */


#ifndef __Cables_FWD_DEFINED__
#define __Cables_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cables Cables;
#else
typedef struct Cables Cables;
#endif /* __cplusplus */

#endif 	/* __Cables_FWD_DEFINED__ */


#ifndef __Types_FWD_DEFINED__
#define __Types_FWD_DEFINED__

#ifdef __cplusplus
typedef class Types Types;
#else
typedef struct Types Types;
#endif /* __cplusplus */

#endif 	/* __Types_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_cticables2_0000 */
/* [local] */ 

typedef /* [public][helpstring][version][uuid] */ 
enum __MIDL___MIDL_itf_cticables2_0000_0001
    {	IDFLT_TIMEOUT	= 15,
	IDFLT_DELAY	= 10
    }	IDefaults;

typedef /* [public][public][public][public][public][helpstring][version][uuid] */ 
enum __MIDL___MIDL_itf_cticables2_0000_0002
    {	ICABLE_NUL	= 0,
	ICABLE_GRY	= 1,
	ICABLE_BLK	= 2,
	ICABLE_PAR	= 3,
	ICABLE_SLV	= 4,
	ICABLE_USB	= 5,
	ICABLE_VTI	= 6,
	ICABLE_TIE	= 7,
	ICABLE_VTL	= 8,
	ICABLE_ILP	= 9
    }	ICableModel;

typedef /* [public][public][public][public][public][helpstring][version][uuid] */ 
enum __MIDL___MIDL_itf_cticables2_0000_0003
    {	IPORT_0	= 0,
	IPORT_1	= 1,
	IPORT_2	= 2,
	IPORT_3	= 3,
	IPORT_4	= 4
    }	ICablePort;

typedef /* [helpstring][version][uuid] */ long ICableHandle;



extern RPC_IF_HANDLE __MIDL_itf_cticables2_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_cticables2_0000_v0_0_s_ifspec;

#ifndef __ICables_INTERFACE_DEFINED__
#define __ICables_INTERFACE_DEFINED__

/* interface ICables */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICables;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("295A0BCE-428D-4251-B432-9DE5100205E5")
    ICables : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LibraryInit( 
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LibraryExit( 
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VersionGet( 
            /* [retval][out] */ BSTR __RPC_FAR *version) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorCode( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HandleNew( 
            /* [in] */ ICableModel model,
            /* [in] */ ICablePort port,
            /* [retval][out] */ long __RPC_FAR *handle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HandleDel( 
            /* [in] */ long handle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OptionsSetTimeout( 
            /* [in] */ long handle,
            /* [in] */ short timeout,
            /* [retval][out] */ short __RPC_FAR *old_timeout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OptionsSetDelay( 
            /* [in] */ long handle,
            /* [in] */ short delay,
            /* [retval][out] */ short __RPC_FAR *old_delay) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ModelGet( 
            /* [in] */ long handle,
            /* [retval][out] */ ICableModel __RPC_FAR *model) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PortGet( 
            /* [in] */ long handle,
            /* [retval][out] */ ICablePort __RPC_FAR *port) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HandleShow( 
            /* [in] */ long handle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableOpen( 
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableClose( 
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableReset( 
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableProbe( 
            /* [in] */ long handle,
            /* [out] */ BOOL __RPC_FAR *result,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableSend( 
            /* [in] */ long handle,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
            /* [in] */ long count,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableRecv( 
            /* [in] */ long handle,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
            /* [in] */ long count,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableCheck( 
            /* [in] */ long handle,
            /* [out] */ short __RPC_FAR *status,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableSetD0( 
            /* [in] */ long handle,
            /* [in] */ BOOL state) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableSetD1( 
            /* [in] */ long handle,
            /* [in] */ BOOL state) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableGetD0( 
            /* [in] */ long handle,
            /* [retval][out] */ BOOL __RPC_FAR *state) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableGetD1( 
            /* [in] */ long handle,
            /* [retval][out] */ BOOL __RPC_FAR *state) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProgressReset( 
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProgressGet( 
            /* [in] */ long handle,
            /* [out] */ long __RPC_FAR *count,
            /* [out] */ long __RPC_FAR *msec,
            /* [out] */ float __RPC_FAR *rate,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CablePut( 
            /* [in] */ long handle,
            /* [in] */ unsigned char data,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CableGet( 
            /* [in] */ long handle,
            /* [out] */ unsigned char __RPC_FAR *data,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorGet( 
            /* [in] */ long code,
            /* [out] */ BSTR __RPC_FAR *message,
            /* [retval][out] */ long __RPC_FAR *ret) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICablesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICables __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICables __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICables __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICables __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICables __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICables __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICables __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LibraryInit )( 
            ICables __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LibraryExit )( 
            ICables __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VersionGet )( 
            ICables __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *version);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ErrorCode )( 
            ICables __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleNew )( 
            ICables __RPC_FAR * This,
            /* [in] */ ICableModel model,
            /* [in] */ ICablePort port,
            /* [retval][out] */ long __RPC_FAR *handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleDel )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OptionsSetTimeout )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ short timeout,
            /* [retval][out] */ short __RPC_FAR *old_timeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OptionsSetDelay )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ short delay,
            /* [retval][out] */ short __RPC_FAR *old_delay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ModelGet )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ ICableModel __RPC_FAR *model);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PortGet )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ ICablePort __RPC_FAR *port);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleShow )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableOpen )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableClose )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableReset )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableProbe )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [out] */ BOOL __RPC_FAR *result,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableSend )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
            /* [in] */ long count,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableRecv )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
            /* [in] */ long count,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableCheck )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [out] */ short __RPC_FAR *status,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableSetD0 )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ BOOL state);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableSetD1 )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ BOOL state);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableGetD0 )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ BOOL __RPC_FAR *state);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableGetD1 )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ BOOL __RPC_FAR *state);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProgressReset )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProgressGet )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [out] */ long __RPC_FAR *count,
            /* [out] */ long __RPC_FAR *msec,
            /* [out] */ float __RPC_FAR *rate,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CablePut )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [in] */ unsigned char data,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CableGet )( 
            ICables __RPC_FAR * This,
            /* [in] */ long handle,
            /* [out] */ unsigned char __RPC_FAR *data,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ErrorGet )( 
            ICables __RPC_FAR * This,
            /* [in] */ long code,
            /* [out] */ BSTR __RPC_FAR *message,
            /* [retval][out] */ long __RPC_FAR *ret);
        
        END_INTERFACE
    } ICablesVtbl;

    interface ICables
    {
        CONST_VTBL struct ICablesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICables_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICables_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICables_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICables_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICables_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICables_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICables_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICables_LibraryInit(This,ret)	\
    (This)->lpVtbl -> LibraryInit(This,ret)

#define ICables_LibraryExit(This,ret)	\
    (This)->lpVtbl -> LibraryExit(This,ret)

#define ICables_VersionGet(This,version)	\
    (This)->lpVtbl -> VersionGet(This,version)

#define ICables_get_ErrorCode(This,pVal)	\
    (This)->lpVtbl -> get_ErrorCode(This,pVal)

#define ICables_HandleNew(This,model,port,handle)	\
    (This)->lpVtbl -> HandleNew(This,model,port,handle)

#define ICables_HandleDel(This,handle)	\
    (This)->lpVtbl -> HandleDel(This,handle)

#define ICables_OptionsSetTimeout(This,handle,timeout,old_timeout)	\
    (This)->lpVtbl -> OptionsSetTimeout(This,handle,timeout,old_timeout)

#define ICables_OptionsSetDelay(This,handle,delay,old_delay)	\
    (This)->lpVtbl -> OptionsSetDelay(This,handle,delay,old_delay)

#define ICables_ModelGet(This,handle,model)	\
    (This)->lpVtbl -> ModelGet(This,handle,model)

#define ICables_PortGet(This,handle,port)	\
    (This)->lpVtbl -> PortGet(This,handle,port)

#define ICables_HandleShow(This,handle)	\
    (This)->lpVtbl -> HandleShow(This,handle)

#define ICables_CableOpen(This,handle,ret)	\
    (This)->lpVtbl -> CableOpen(This,handle,ret)

#define ICables_CableClose(This,handle,ret)	\
    (This)->lpVtbl -> CableClose(This,handle,ret)

#define ICables_CableReset(This,handle,ret)	\
    (This)->lpVtbl -> CableReset(This,handle,ret)

#define ICables_CableProbe(This,handle,result,ret)	\
    (This)->lpVtbl -> CableProbe(This,handle,result,ret)

#define ICables_CableSend(This,handle,data,count,ret)	\
    (This)->lpVtbl -> CableSend(This,handle,data,count,ret)

#define ICables_CableRecv(This,handle,data,count,ret)	\
    (This)->lpVtbl -> CableRecv(This,handle,data,count,ret)

#define ICables_CableCheck(This,handle,status,ret)	\
    (This)->lpVtbl -> CableCheck(This,handle,status,ret)

#define ICables_CableSetD0(This,handle,state)	\
    (This)->lpVtbl -> CableSetD0(This,handle,state)

#define ICables_CableSetD1(This,handle,state)	\
    (This)->lpVtbl -> CableSetD1(This,handle,state)

#define ICables_CableGetD0(This,handle,state)	\
    (This)->lpVtbl -> CableGetD0(This,handle,state)

#define ICables_CableGetD1(This,handle,state)	\
    (This)->lpVtbl -> CableGetD1(This,handle,state)

#define ICables_ProgressReset(This,handle,ret)	\
    (This)->lpVtbl -> ProgressReset(This,handle,ret)

#define ICables_ProgressGet(This,handle,count,msec,rate,ret)	\
    (This)->lpVtbl -> ProgressGet(This,handle,count,msec,rate,ret)

#define ICables_CablePut(This,handle,data,ret)	\
    (This)->lpVtbl -> CablePut(This,handle,data,ret)

#define ICables_CableGet(This,handle,data,ret)	\
    (This)->lpVtbl -> CableGet(This,handle,data,ret)

#define ICables_ErrorGet(This,code,message,ret)	\
    (This)->lpVtbl -> ErrorGet(This,code,message,ret)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_LibraryInit_Proxy( 
    ICables __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_LibraryInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_LibraryExit_Proxy( 
    ICables __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_LibraryExit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_VersionGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *version);


void __RPC_STUB ICables_VersionGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICables_get_ErrorCode_Proxy( 
    ICables __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB ICables_get_ErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_HandleNew_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ ICableModel model,
    /* [in] */ ICablePort port,
    /* [retval][out] */ long __RPC_FAR *handle);


void __RPC_STUB ICables_HandleNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_HandleDel_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle);


void __RPC_STUB ICables_HandleDel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_OptionsSetTimeout_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ short timeout,
    /* [retval][out] */ short __RPC_FAR *old_timeout);


void __RPC_STUB ICables_OptionsSetTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_OptionsSetDelay_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ short delay,
    /* [retval][out] */ short __RPC_FAR *old_delay);


void __RPC_STUB ICables_OptionsSetDelay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_ModelGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ ICableModel __RPC_FAR *model);


void __RPC_STUB ICables_ModelGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_PortGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ ICablePort __RPC_FAR *port);


void __RPC_STUB ICables_PortGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_HandleShow_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle);


void __RPC_STUB ICables_HandleShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableOpen_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableClose_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableReset_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableReset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableProbe_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ BOOL __RPC_FAR *result,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableProbe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableSend_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
    /* [in] */ long count,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableSend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableRecv_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *data,
    /* [in] */ long count,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableRecv_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableCheck_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ short __RPC_FAR *status,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableSetD0_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ BOOL state);


void __RPC_STUB ICables_CableSetD0_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableSetD1_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ BOOL state);


void __RPC_STUB ICables_CableSetD1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableGetD0_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ BOOL __RPC_FAR *state);


void __RPC_STUB ICables_CableGetD0_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableGetD1_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ BOOL __RPC_FAR *state);


void __RPC_STUB ICables_CableGetD1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_ProgressReset_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_ProgressReset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_ProgressGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ long __RPC_FAR *count,
    /* [out] */ long __RPC_FAR *msec,
    /* [out] */ float __RPC_FAR *rate,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_ProgressGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CablePut_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [in] */ unsigned char data,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CablePut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ unsigned char __RPC_FAR *data,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_CableGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_ErrorGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long code,
    /* [out] */ BSTR __RPC_FAR *message,
    /* [retval][out] */ long __RPC_FAR *ret);


void __RPC_STUB ICables_ErrorGet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICables_INTERFACE_DEFINED__ */


#ifndef __ITypes_INTERFACE_DEFINED__
#define __ITypes_INTERFACE_DEFINED__

/* interface ITypes */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITypes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03353B2B-A497-4B42-A6F6-892FB945BA7E")
    ITypes : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ModelToString( 
            /* [in] */ ICableModel model,
            /* [retval][out] */ BSTR __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StringToModel( 
            /* [in] */ BSTR model,
            /* [retval][out] */ ICableModel __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PortToString( 
            /* [in] */ ICablePort port,
            /* [retval][out] */ BSTR __RPC_FAR *ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StringToPort( 
            /* [in] */ BSTR port,
            /* [retval][out] */ ICablePort __RPC_FAR *ret) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITypesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITypes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITypes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITypes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITypes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITypes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITypes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITypes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ModelToString )( 
            ITypes __RPC_FAR * This,
            /* [in] */ ICableModel model,
            /* [retval][out] */ BSTR __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StringToModel )( 
            ITypes __RPC_FAR * This,
            /* [in] */ BSTR model,
            /* [retval][out] */ ICableModel __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PortToString )( 
            ITypes __RPC_FAR * This,
            /* [in] */ ICablePort port,
            /* [retval][out] */ BSTR __RPC_FAR *ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StringToPort )( 
            ITypes __RPC_FAR * This,
            /* [in] */ BSTR port,
            /* [retval][out] */ ICablePort __RPC_FAR *ret);
        
        END_INTERFACE
    } ITypesVtbl;

    interface ITypes
    {
        CONST_VTBL struct ITypesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITypes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITypes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITypes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITypes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITypes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITypes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITypes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITypes_ModelToString(This,model,ret)	\
    (This)->lpVtbl -> ModelToString(This,model,ret)

#define ITypes_StringToModel(This,model,ret)	\
    (This)->lpVtbl -> StringToModel(This,model,ret)

#define ITypes_PortToString(This,port,ret)	\
    (This)->lpVtbl -> PortToString(This,port,ret)

#define ITypes_StringToPort(This,port,ret)	\
    (This)->lpVtbl -> StringToPort(This,port,ret)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITypes_ModelToString_Proxy( 
    ITypes __RPC_FAR * This,
    /* [in] */ ICableModel model,
    /* [retval][out] */ BSTR __RPC_FAR *ret);


void __RPC_STUB ITypes_ModelToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITypes_StringToModel_Proxy( 
    ITypes __RPC_FAR * This,
    /* [in] */ BSTR model,
    /* [retval][out] */ ICableModel __RPC_FAR *ret);


void __RPC_STUB ITypes_StringToModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITypes_PortToString_Proxy( 
    ITypes __RPC_FAR * This,
    /* [in] */ ICablePort port,
    /* [retval][out] */ BSTR __RPC_FAR *ret);


void __RPC_STUB ITypes_PortToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITypes_StringToPort_Proxy( 
    ITypes __RPC_FAR * This,
    /* [in] */ BSTR port,
    /* [retval][out] */ ICablePort __RPC_FAR *ret);


void __RPC_STUB ITypes_StringToPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITypes_INTERFACE_DEFINED__ */



#ifndef __CTICABLES2Lib_LIBRARY_DEFINED__
#define __CTICABLES2Lib_LIBRARY_DEFINED__

/* library CTICABLES2Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CTICABLES2Lib;

EXTERN_C const CLSID CLSID_Cables;

#ifdef __cplusplus

class DECLSPEC_UUID("EA25011A-AC84-4498-9E50-05EBC57AE012")
Cables;
#endif

EXTERN_C const CLSID CLSID_Types;

#ifdef __cplusplus

class DECLSPEC_UUID("4766347A-54D0-4F60-B43A-8F8F5618ECFE")
Types;
#endif
#endif /* __CTICABLES2Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long __RPC_FAR *, unsigned long            , LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
