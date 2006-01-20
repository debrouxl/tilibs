/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Jan 20 10:51:59 2006
 */
/* Compiler settings for C:\sources\roms\ticables2\com\cticables2.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "cticables2.h"

#define TYPE_FORMAT_STRING_SIZE   1045                              
#define PROC_FORMAT_STRING_SIZE   1109                              

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Standard interface: __MIDL_itf_cticables2_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICables, ver. 0.0,
   GUID={0x295A0BCE,0x428D,0x4251,{0xB4,0x32,0x9D,0xE5,0x10,0x02,0x05,0xE5}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ICables_ServerInfo;

#pragma code_seg(".orpc")
/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_CableGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long handle,
    /* [out] */ unsigned char __RPC_FAR *data,
    /* [retval][out] */ long __RPC_FAR *ret)
{
CLIENT_CALL_RETURN _RetVal;


#if defined( _ALPHA_ )
    va_list vlist;
#endif
    
#if defined( _ALPHA_ )
    va_start(vlist,ret);
    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[892],
                  vlist.a0);
#elif defined( _PPC_ ) || defined( _MIPS_ )

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[892],
                  ( unsigned char __RPC_FAR * )&This,
                  ( unsigned char __RPC_FAR * )&handle,
                  ( unsigned char __RPC_FAR * )&data,
                  ( unsigned char __RPC_FAR * )&ret);
#else
    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[892],
                  ( unsigned char __RPC_FAR * )&This);
#endif
    return ( HRESULT  )_RetVal.Simple;
    
}

/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICables_ErrorGet_Proxy( 
    ICables __RPC_FAR * This,
    /* [in] */ long code,
    /* [out] */ BSTR __RPC_FAR *message,
    /* [retval][out] */ long __RPC_FAR *ret)
{
CLIENT_CALL_RETURN _RetVal;


#if defined( _ALPHA_ )
    va_list vlist;
#endif
    
#if defined( _ALPHA_ )
    va_start(vlist,ret);
    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[932],
                  vlist.a0);
#elif defined( _PPC_ ) || defined( _MIPS_ )

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[932],
                  ( unsigned char __RPC_FAR * )&This,
                  ( unsigned char __RPC_FAR * )&code,
                  ( unsigned char __RPC_FAR * )&message,
                  ( unsigned char __RPC_FAR * )&ret);
#else
    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Object_StubDesc,
                  (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[932],
                  ( unsigned char __RPC_FAR * )&This);
#endif
    return ( HRESULT  )_RetVal.Simple;
    
}

static const unsigned short ICables_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    56,
    84,
    112,
    152,
    180,
    220,
    260,
    294,
    328,
    356,
    390,
    424,
    458,
    498,
    544,
    590,
    630,
    664,
    698,
    732,
    766,
    800,
    852,
    892,
    932
    };

static const MIDL_SERVER_INFO ICables_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ICables_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ICables_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ICables_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(34) _ICablesProxyVtbl = 
{
    &ICables_ProxyInfo,
    &IID_ICables,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* ICables::LibraryInit */ ,
    (void *)-1 /* ICables::LibraryExit */ ,
    (void *)-1 /* ICables::VersionGet */ ,
    (void *)-1 /* ICables::get_ErrorCode */ ,
    (void *)-1 /* ICables::HandleNew */ ,
    (void *)-1 /* ICables::HandleDel */ ,
    (void *)-1 /* ICables::OptionsSetTimeout */ ,
    (void *)-1 /* ICables::OptionsSetDelay */ ,
    (void *)-1 /* ICables::ModelGet */ ,
    (void *)-1 /* ICables::PortGet */ ,
    (void *)-1 /* ICables::HandleShow */ ,
    (void *)-1 /* ICables::CableOpen */ ,
    (void *)-1 /* ICables::CableClose */ ,
    (void *)-1 /* ICables::CableReset */ ,
    (void *)-1 /* ICables::CableProbe */ ,
    (void *)-1 /* ICables::CableSend */ ,
    (void *)-1 /* ICables::CableRecv */ ,
    (void *)-1 /* ICables::CableCheck */ ,
    (void *)-1 /* ICables::CableSetD0 */ ,
    (void *)-1 /* ICables::CableSetD1 */ ,
    (void *)-1 /* ICables::CableGetD0 */ ,
    (void *)-1 /* ICables::CableGetD1 */ ,
    (void *)-1 /* ICables::ProgressReset */ ,
    (void *)-1 /* ICables::ProgressGet */ ,
    (void *)-1 /* ICables::CablePut */ ,
    ICables_CableGet_Proxy ,
    ICables_ErrorGet_Proxy
};


static const PRPC_STUB_FUNCTION ICables_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ICablesStubVtbl =
{
    &IID_ICables,
    &ICables_ServerInfo,
    34,
    &ICables_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ITypes, ver. 0.0,
   GUID={0x03353B2B,0xA497,0x4B42,{0xA6,0xF6,0x89,0x2F,0xB9,0x45,0xBA,0x7E}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITypes_ServerInfo;

#pragma code_seg(".orpc")
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2];

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static const unsigned short ITypes_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    972,
    1006,
    1040,
    1074
    };

static const MIDL_SERVER_INFO ITypes_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ITypes_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ITypes_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ITypes_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(11) _ITypesProxyVtbl = 
{
    &ITypes_ProxyInfo,
    &IID_ITypes,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* ITypes::ModelToString */ ,
    (void *)-1 /* ITypes::StringToModel */ ,
    (void *)-1 /* ITypes::PortToString */ ,
    (void *)-1 /* ITypes::StringToPort */
};


static const PRPC_STUB_FUNCTION ITypes_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ITypesStubVtbl =
{
    &IID_ITypes,
    &ITypes_ServerInfo,
    11,
    &ITypes_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

#pragma data_seg(".rdata")

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure LibraryInit */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/*  8 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x10 ),	/* 16 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter ret */

/* 16 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 18 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 24 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LibraryExit */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 36 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x10 ),	/* 16 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter ret */

/* 44 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 46 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 52 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure VersionGet */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 64 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter version */

/* 72 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 74 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 76 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 80 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_ErrorCode */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 92 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x10 ),	/* 16 */
/* 98 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 100 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 102 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 108 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure HandleNew */

/* 112 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 120 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 122 */	NdrFcShort( 0xc ),	/* 12 */
/* 124 */	NdrFcShort( 0x10 ),	/* 16 */
/* 126 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter model */

/* 128 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 130 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 132 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter port */

/* 134 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 136 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 138 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter handle */

/* 140 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 142 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 148 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure HandleDel */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 160 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter handle */

/* 168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 170 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 176 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OptionsSetTimeout */

/* 180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0xd ),	/* 13 */
#ifndef _ALPHA_
/* 188 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 190 */	NdrFcShort( 0xe ),	/* 14 */
/* 192 */	NdrFcShort( 0xe ),	/* 14 */
/* 194 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 196 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 198 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter timeout */

/* 202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 204 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 206 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter old_timeout */

/* 208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 210 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 212 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 216 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OptionsSetDelay */

/* 220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0xe ),	/* 14 */
#ifndef _ALPHA_
/* 228 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 230 */	NdrFcShort( 0xe ),	/* 14 */
/* 232 */	NdrFcShort( 0xe ),	/* 14 */
/* 234 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 236 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 238 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter delay */

/* 242 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 244 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 246 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter old_delay */

/* 248 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 250 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 252 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 256 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ModelGet */

/* 260 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0xf ),	/* 15 */
#ifndef _ALPHA_
/* 268 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	NdrFcShort( 0xe ),	/* 14 */
/* 274 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 278 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter model */

/* 282 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
#ifndef _ALPHA_
/* 284 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 286 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 290 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PortGet */

/* 294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x10 ),	/* 16 */
#ifndef _ALPHA_
/* 302 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 304 */	NdrFcShort( 0x8 ),	/* 8 */
/* 306 */	NdrFcShort( 0xe ),	/* 14 */
/* 308 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 312 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter port */

/* 316 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
#ifndef _ALPHA_
/* 318 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 320 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 324 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure HandleShow */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x11 ),	/* 17 */
#ifndef _ALPHA_
/* 336 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter handle */

/* 344 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 346 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 350 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 352 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableOpen */

/* 356 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x12 ),	/* 18 */
#ifndef _ALPHA_
/* 364 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	NdrFcShort( 0x10 ),	/* 16 */
/* 370 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 374 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 378 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 380 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 386 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableClose */

/* 390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x13 ),	/* 19 */
#ifndef _ALPHA_
/* 398 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 402 */	NdrFcShort( 0x10 ),	/* 16 */
/* 404 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 406 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 408 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 412 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 414 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 420 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableReset */

/* 424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x14 ),	/* 20 */
#ifndef _ALPHA_
/* 432 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 436 */	NdrFcShort( 0x10 ),	/* 16 */
/* 438 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 440 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 442 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 446 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 448 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 454 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableProbe */

/* 458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x15 ),	/* 21 */
#ifndef _ALPHA_
/* 466 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 468 */	NdrFcShort( 0x8 ),	/* 8 */
/* 470 */	NdrFcShort( 0x18 ),	/* 24 */
/* 472 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 476 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter result */

/* 480 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 482 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 486 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 488 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 492 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 494 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 496 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableSend */

/* 498 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 500 */	NdrFcLong( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0x16 ),	/* 22 */
#ifndef _ALPHA_
/* 506 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 508 */	NdrFcShort( 0x10 ),	/* 16 */
/* 510 */	NdrFcShort( 0x10 ),	/* 16 */
/* 512 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter handle */

/* 514 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 516 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 518 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter data */

/* 520 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 522 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 524 */	NdrFcShort( 0x3de ),	/* Type Offset=990 */

	/* Parameter count */

/* 526 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 528 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 530 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 532 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 534 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 536 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 538 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 540 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableRecv */

/* 544 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x17 ),	/* 23 */
#ifndef _ALPHA_
/* 552 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 554 */	NdrFcShort( 0x10 ),	/* 16 */
/* 556 */	NdrFcShort( 0x10 ),	/* 16 */
/* 558 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter handle */

/* 560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 562 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter data */

/* 566 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
#ifndef _ALPHA_
/* 568 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 570 */	NdrFcShort( 0x3f4 ),	/* Type Offset=1012 */

	/* Parameter count */

/* 572 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 574 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 580 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 586 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableCheck */

/* 590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 596 */	NdrFcShort( 0x18 ),	/* 24 */
#ifndef _ALPHA_
/* 598 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 602 */	NdrFcShort( 0x16 ),	/* 22 */
/* 604 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 608 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter status */

/* 612 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 614 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 616 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter ret */

/* 618 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 620 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 626 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableSetD0 */

/* 630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 636 */	NdrFcShort( 0x19 ),	/* 25 */
#ifndef _ALPHA_
/* 638 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 640 */	NdrFcShort( 0x10 ),	/* 16 */
/* 642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 644 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 648 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 652 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 654 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 658 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 660 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 662 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableSetD1 */

/* 664 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 666 */	NdrFcLong( 0x0 ),	/* 0 */
/* 670 */	NdrFcShort( 0x1a ),	/* 26 */
#ifndef _ALPHA_
/* 672 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 674 */	NdrFcShort( 0x10 ),	/* 16 */
/* 676 */	NdrFcShort( 0x8 ),	/* 8 */
/* 678 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 680 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 682 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 684 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 686 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 688 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 692 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 694 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableGetD0 */

/* 698 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x1b ),	/* 27 */
#ifndef _ALPHA_
/* 706 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 710 */	NdrFcShort( 0x10 ),	/* 16 */
/* 712 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 714 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 716 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 718 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 720 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 722 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 728 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableGetD1 */

/* 732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 738 */	NdrFcShort( 0x1c ),	/* 28 */
#ifndef _ALPHA_
/* 740 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 744 */	NdrFcShort( 0x10 ),	/* 16 */
/* 746 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 750 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 754 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 756 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 760 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 762 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 764 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProgressReset */

/* 766 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 768 */	NdrFcLong( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x1d ),	/* 29 */
#ifndef _ALPHA_
/* 774 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 778 */	NdrFcShort( 0x10 ),	/* 16 */
/* 780 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter handle */

/* 782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 784 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ret */

/* 788 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 790 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 796 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProgressGet */

/* 800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 806 */	NdrFcShort( 0x1e ),	/* 30 */
#ifndef _ALPHA_
/* 808 */	NdrFcShort( 0x1c ),	/* x86, MIPS, PPC Stack size/offset = 28 */
#else
			NdrFcShort( 0x38 ),	/* Alpha Stack size/offset = 56 */
#endif
/* 810 */	NdrFcShort( 0x8 ),	/* 8 */
/* 812 */	NdrFcShort( 0x28 ),	/* 40 */
/* 814 */	0x4,		/* Oi2 Flags:  has return, */
			0x6,		/* 6 */

	/* Parameter handle */

/* 816 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 818 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter count */

/* 822 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 824 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter msec */

/* 828 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 830 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rate */

/* 834 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 836 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 838 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter ret */

/* 840 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 842 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 846 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 848 */	NdrFcShort( 0x18 ),	/* x86, MIPS, PPC Stack size/offset = 24 */
#else
			NdrFcShort( 0x30 ),	/* Alpha Stack size/offset = 48 */
#endif
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CablePut */

/* 852 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 854 */	NdrFcLong( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x1f ),	/* 31 */
#ifndef _ALPHA_
/* 860 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 862 */	NdrFcShort( 0xd ),	/* 13 */
/* 864 */	NdrFcShort( 0x10 ),	/* 16 */
/* 866 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 868 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 870 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 872 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter data */

/* 874 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 876 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 878 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter ret */

/* 880 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 882 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 886 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 888 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 890 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CableGet */

/* 892 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 898 */	NdrFcShort( 0x20 ),	/* 32 */
#ifndef _ALPHA_
/* 900 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 904 */	NdrFcShort( 0x15 ),	/* 21 */
/* 906 */	0x4,		/* Oi2 Flags:  has return, */
			0x4,		/* 4 */

	/* Parameter handle */

/* 908 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 910 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter data */

/* 914 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 916 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 918 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter ret */

/* 920 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 922 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 926 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 928 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ErrorGet */

/* 932 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 934 */	NdrFcLong( 0x0 ),	/* 0 */
/* 938 */	NdrFcShort( 0x21 ),	/* 33 */
#ifndef _ALPHA_
/* 940 */	NdrFcShort( 0x14 ),	/* x86, MIPS, PPC Stack size/offset = 20 */
#else
			NdrFcShort( 0x28 ),	/* Alpha Stack size/offset = 40 */
#endif
/* 942 */	NdrFcShort( 0x8 ),	/* 8 */
/* 944 */	NdrFcShort( 0x10 ),	/* 16 */
/* 946 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter code */

/* 948 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 950 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 952 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter message */

/* 954 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 956 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 958 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Parameter ret */

/* 960 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 962 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 968 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ModelToString */

/* 972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/* 980 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 982 */	NdrFcShort( 0x6 ),	/* 6 */
/* 984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 986 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter model */

/* 988 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 990 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 992 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ret */

/* 994 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 996 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 998 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 1000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1002 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure StringToModel */

/* 1006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 1014 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0xe ),	/* 14 */
/* 1020 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter model */

/* 1022 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
#ifndef _ALPHA_
/* 1024 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1026 */	NdrFcShort( 0x40a ),	/* Type Offset=1034 */

	/* Parameter ret */

/* 1028 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
#ifndef _ALPHA_
/* 1030 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1032 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Return value */

/* 1034 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1036 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1038 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PortToString */

/* 1040 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1042 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 1048 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1050 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1052 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1054 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter port */

/* 1056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 1058 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1060 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter ret */

/* 1062 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 1064 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1066 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 1068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1070 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure StringToPort */

/* 1074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 1082 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 1084 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1086 */	NdrFcShort( 0xe ),	/* 14 */
/* 1088 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter port */

/* 1090 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
#ifndef _ALPHA_
/* 1092 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 1094 */	NdrFcShort( 0x40a ),	/* Type Offset=1034 */

	/* Parameter ret */

/* 1096 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
#ifndef _ALPHA_
/* 1098 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 1100 */	NdrFcShort( 0x30 ),	/* Type Offset=48 */

	/* Return value */

/* 1102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 1104 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 1106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  8 */	NdrFcShort( 0x1a ),	/* Offset= 26 (34) */
/* 10 */	
			0x13, 0x0,	/* FC_OP */
/* 12 */	NdrFcShort( 0xc ),	/* Offset= 12 (24) */
/* 14 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 16 */	NdrFcShort( 0x2 ),	/* 2 */
/* 18 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 20 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 22 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 24 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 26 */	NdrFcShort( 0x8 ),	/* 8 */
/* 28 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (14) */
/* 30 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 32 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 34 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0x4 ),	/* 4 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (10) */
/* 44 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 46 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 48 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 50 */	0xd,		/* FC_ENUM16 */
			0x5c,		/* FC_PAD */
/* 52 */	
			0x11, 0x0,	/* FC_RP */
/* 54 */	NdrFcShort( 0x3a8 ),	/* Offset= 936 (990) */
/* 56 */	
			0x12, 0x10,	/* FC_UP */
/* 58 */	NdrFcShort( 0x2 ),	/* Offset= 2 (60) */
/* 60 */	
			0x12, 0x0,	/* FC_UP */
/* 62 */	NdrFcShort( 0x38e ),	/* Offset= 910 (972) */
/* 64 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 66 */	NdrFcShort( 0x18 ),	/* 24 */
/* 68 */	NdrFcShort( 0xa ),	/* 10 */
/* 70 */	NdrFcLong( 0x8 ),	/* 8 */
/* 74 */	NdrFcShort( 0x58 ),	/* Offset= 88 (162) */
/* 76 */	NdrFcLong( 0xd ),	/* 13 */
/* 80 */	NdrFcShort( 0x8a ),	/* Offset= 138 (218) */
/* 82 */	NdrFcLong( 0x9 ),	/* 9 */
/* 86 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (270) */
/* 88 */	NdrFcLong( 0xc ),	/* 12 */
/* 92 */	NdrFcShort( 0x27e ),	/* Offset= 638 (730) */
/* 94 */	NdrFcLong( 0x24 ),	/* 36 */
/* 98 */	NdrFcShort( 0x2a6 ),	/* Offset= 678 (776) */
/* 100 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 104 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (810) */
/* 106 */	NdrFcLong( 0x10 ),	/* 16 */
/* 110 */	NdrFcShort( 0x2da ),	/* Offset= 730 (840) */
/* 112 */	NdrFcLong( 0x2 ),	/* 2 */
/* 116 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (870) */
/* 118 */	NdrFcLong( 0x3 ),	/* 3 */
/* 122 */	NdrFcShort( 0x30a ),	/* Offset= 778 (900) */
/* 124 */	NdrFcLong( 0x14 ),	/* 20 */
/* 128 */	NdrFcShort( 0x322 ),	/* Offset= 802 (930) */
/* 130 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (129) */
/* 132 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 136 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 142 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x1 ),	/* 1 */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 154 */	0x12, 0x0,	/* FC_UP */
/* 156 */	NdrFcShort( 0xffffff7c ),	/* Offset= -132 (24) */
/* 158 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 160 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 162 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 168 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 172 */	NdrFcShort( 0x4 ),	/* 4 */
/* 174 */	0x11, 0x0,	/* FC_RP */
/* 176 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (132) */
/* 178 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 180 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 182 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 184 */	NdrFcLong( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 194 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 196 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 198 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 200 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 204 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 212 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 214 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (182) */
/* 216 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 218 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x6 ),	/* Offset= 6 (230) */
/* 226 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 230 */	
			0x11, 0x0,	/* FC_RP */
/* 232 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (200) */
/* 234 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 236 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 246 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 248 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 250 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 252 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 264 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 266 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (234) */
/* 268 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 270 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x6 ),	/* Offset= 6 (282) */
/* 278 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 280 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 282 */	
			0x11, 0x0,	/* FC_RP */
/* 284 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (252) */
/* 286 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 288 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 290 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 292 */	NdrFcShort( 0x2 ),	/* Offset= 2 (294) */
/* 294 */	NdrFcShort( 0x10 ),	/* 16 */
/* 296 */	NdrFcShort( 0x2b ),	/* 43 */
/* 298 */	NdrFcLong( 0x3 ),	/* 3 */
/* 302 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 304 */	NdrFcLong( 0x11 ),	/* 17 */
/* 308 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 310 */	NdrFcLong( 0x2 ),	/* 2 */
/* 314 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 316 */	NdrFcLong( 0x4 ),	/* 4 */
/* 320 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 322 */	NdrFcLong( 0x5 ),	/* 5 */
/* 326 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 328 */	NdrFcLong( 0xb ),	/* 11 */
/* 332 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 334 */	NdrFcLong( 0xa ),	/* 10 */
/* 338 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 340 */	NdrFcLong( 0x6 ),	/* 6 */
/* 344 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (558) */
/* 346 */	NdrFcLong( 0x7 ),	/* 7 */
/* 350 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 352 */	NdrFcLong( 0x8 ),	/* 8 */
/* 356 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (564) */
/* 358 */	NdrFcLong( 0xd ),	/* 13 */
/* 362 */	NdrFcShort( 0xffffff4c ),	/* Offset= -180 (182) */
/* 364 */	NdrFcLong( 0x9 ),	/* 9 */
/* 368 */	NdrFcShort( 0xffffff7a ),	/* Offset= -134 (234) */
/* 370 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 374 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (568) */
/* 376 */	NdrFcLong( 0x24 ),	/* 36 */
/* 380 */	NdrFcShort( 0xc0 ),	/* Offset= 192 (572) */
/* 382 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 386 */	NdrFcShort( 0xba ),	/* Offset= 186 (572) */
/* 388 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 392 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (624) */
/* 394 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 398 */	NdrFcShort( 0xe6 ),	/* Offset= 230 (628) */
/* 400 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 404 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (632) */
/* 406 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 410 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (636) */
/* 412 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 416 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (640) */
/* 418 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 422 */	NdrFcShort( 0xce ),	/* Offset= 206 (628) */
/* 424 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 428 */	NdrFcShort( 0xcc ),	/* Offset= 204 (632) */
/* 430 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 434 */	NdrFcShort( 0xd2 ),	/* Offset= 210 (644) */
/* 436 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 440 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (640) */
/* 442 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 446 */	NdrFcShort( 0xca ),	/* Offset= 202 (648) */
/* 448 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 452 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (652) */
/* 454 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 458 */	NdrFcShort( 0xc6 ),	/* Offset= 198 (656) */
/* 460 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 464 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (660) */
/* 466 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 470 */	NdrFcShort( 0xbe ),	/* Offset= 190 (660) */
/* 472 */	NdrFcLong( 0x10 ),	/* 16 */
/* 476 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 478 */	NdrFcLong( 0x12 ),	/* 18 */
/* 482 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 484 */	NdrFcLong( 0x13 ),	/* 19 */
/* 488 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 490 */	NdrFcLong( 0x16 ),	/* 22 */
/* 494 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 496 */	NdrFcLong( 0x17 ),	/* 23 */
/* 500 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 502 */	NdrFcLong( 0xe ),	/* 14 */
/* 506 */	NdrFcShort( 0x9e ),	/* Offset= 158 (664) */
/* 508 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 512 */	NdrFcShort( 0xa4 ),	/* Offset= 164 (676) */
/* 514 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 518 */	NdrFcShort( 0x6a ),	/* Offset= 106 (624) */
/* 520 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 524 */	NdrFcShort( 0x68 ),	/* Offset= 104 (628) */
/* 526 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 530 */	NdrFcShort( 0x66 ),	/* Offset= 102 (632) */
/* 532 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 536 */	NdrFcShort( 0x60 ),	/* Offset= 96 (632) */
/* 538 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 542 */	NdrFcShort( 0x5a ),	/* Offset= 90 (632) */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x0 ),	/* Offset= 0 (548) */
/* 550 */	NdrFcLong( 0x1 ),	/* 1 */
/* 554 */	NdrFcShort( 0x0 ),	/* Offset= 0 (554) */
/* 556 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (555) */
/* 558 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 560 */	NdrFcShort( 0x8 ),	/* 8 */
/* 562 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 564 */	
			0x12, 0x0,	/* FC_UP */
/* 566 */	NdrFcShort( 0xfffffde2 ),	/* Offset= -542 (24) */
/* 568 */	
			0x12, 0x0,	/* FC_UP */
/* 570 */	NdrFcShort( 0x192 ),	/* Offset= 402 (972) */
/* 572 */	
			0x12, 0x0,	/* FC_UP */
/* 574 */	NdrFcShort( 0x1e ),	/* Offset= 30 (604) */
/* 576 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 578 */	NdrFcLong( 0x2f ),	/* 47 */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 588 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 590 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 592 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 594 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 596 */	NdrFcShort( 0x1 ),	/* 1 */
/* 598 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 600 */	NdrFcShort( 0x4 ),	/* 4 */
/* 602 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 604 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 606 */	NdrFcShort( 0x10 ),	/* 16 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0xa ),	/* Offset= 10 (620) */
/* 612 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 614 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 616 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (576) */
/* 618 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 620 */	
			0x12, 0x0,	/* FC_UP */
/* 622 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (594) */
/* 624 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 626 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 628 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 630 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 632 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 634 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 636 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 638 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 640 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 642 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 644 */	
			0x12, 0x0,	/* FC_UP */
/* 646 */	NdrFcShort( 0xffffffa8 ),	/* Offset= -88 (558) */
/* 648 */	
			0x12, 0x10,	/* FC_UP */
/* 650 */	NdrFcShort( 0xffffffaa ),	/* Offset= -86 (564) */
/* 652 */	
			0x12, 0x10,	/* FC_UP */
/* 654 */	NdrFcShort( 0xfffffe28 ),	/* Offset= -472 (182) */
/* 656 */	
			0x12, 0x10,	/* FC_UP */
/* 658 */	NdrFcShort( 0xfffffe58 ),	/* Offset= -424 (234) */
/* 660 */	
			0x12, 0x10,	/* FC_UP */
/* 662 */	NdrFcShort( 0xffffffa2 ),	/* Offset= -94 (568) */
/* 664 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 666 */	NdrFcShort( 0x10 ),	/* 16 */
/* 668 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 670 */	0x2,		/* FC_CHAR */
			0x38,		/* FC_ALIGNM4 */
/* 672 */	0x8,		/* FC_LONG */
			0x39,		/* FC_ALIGNM8 */
/* 674 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 676 */	
			0x12, 0x0,	/* FC_UP */
/* 678 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (664) */
/* 680 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 682 */	NdrFcShort( 0x20 ),	/* 32 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* Offset= 0 (686) */
/* 688 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 690 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 692 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 694 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 696 */	NdrFcShort( 0xfffffe66 ),	/* Offset= -410 (286) */
/* 698 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 700 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x4 ),	/* 4 */
/* 704 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 708 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 710 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 712 */	NdrFcShort( 0x4 ),	/* 4 */
/* 714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 716 */	NdrFcShort( 0x1 ),	/* 1 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 722 */	0x12, 0x0,	/* FC_UP */
/* 724 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (680) */
/* 726 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 728 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 730 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x6 ),	/* Offset= 6 (742) */
/* 738 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 742 */	
			0x11, 0x0,	/* FC_RP */
/* 744 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (700) */
/* 746 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 748 */	NdrFcShort( 0x4 ),	/* 4 */
/* 750 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 756 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 758 */	NdrFcShort( 0x4 ),	/* 4 */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x1 ),	/* 1 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	0x12, 0x0,	/* FC_UP */
/* 770 */	NdrFcShort( 0xffffff5a ),	/* Offset= -166 (604) */
/* 772 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 774 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 776 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x6 ),	/* Offset= 6 (788) */
/* 784 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 786 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 788 */	
			0x11, 0x0,	/* FC_RP */
/* 790 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (746) */
/* 792 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 794 */	NdrFcShort( 0x8 ),	/* 8 */
/* 796 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 798 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 800 */	NdrFcShort( 0x10 ),	/* 16 */
/* 802 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 804 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 806 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (792) */
			0x5b,		/* FC_END */
/* 810 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 812 */	NdrFcShort( 0x18 ),	/* 24 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0xa ),	/* Offset= 10 (826) */
/* 818 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 820 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 822 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (798) */
/* 824 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 826 */	
			0x11, 0x0,	/* FC_RP */
/* 828 */	NdrFcShort( 0xfffffd8c ),	/* Offset= -628 (200) */
/* 830 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 832 */	NdrFcShort( 0x1 ),	/* 1 */
/* 834 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 838 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 840 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 846 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 848 */	NdrFcShort( 0x4 ),	/* 4 */
/* 850 */	NdrFcShort( 0x4 ),	/* 4 */
/* 852 */	0x12, 0x0,	/* FC_UP */
/* 854 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (830) */
/* 856 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 858 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 860 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 862 */	NdrFcShort( 0x2 ),	/* 2 */
/* 864 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 868 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 870 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 872 */	NdrFcShort( 0x8 ),	/* 8 */
/* 874 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 876 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 878 */	NdrFcShort( 0x4 ),	/* 4 */
/* 880 */	NdrFcShort( 0x4 ),	/* 4 */
/* 882 */	0x12, 0x0,	/* FC_UP */
/* 884 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (860) */
/* 886 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 888 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 890 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 892 */	NdrFcShort( 0x4 ),	/* 4 */
/* 894 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 896 */	NdrFcShort( 0x0 ),	/* 0 */
/* 898 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 900 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 904 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 906 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 908 */	NdrFcShort( 0x4 ),	/* 4 */
/* 910 */	NdrFcShort( 0x4 ),	/* 4 */
/* 912 */	0x12, 0x0,	/* FC_UP */
/* 914 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (890) */
/* 916 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 918 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 920 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 922 */	NdrFcShort( 0x8 ),	/* 8 */
/* 924 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 928 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 930 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 932 */	NdrFcShort( 0x8 ),	/* 8 */
/* 934 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 936 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 938 */	NdrFcShort( 0x4 ),	/* 4 */
/* 940 */	NdrFcShort( 0x4 ),	/* 4 */
/* 942 */	0x12, 0x0,	/* FC_UP */
/* 944 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (920) */
/* 946 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 948 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 950 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 954 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 956 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 958 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 960 */	NdrFcShort( 0x8 ),	/* 8 */
/* 962 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 964 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 966 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 968 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (950) */
/* 970 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 972 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 974 */	NdrFcShort( 0x28 ),	/* 40 */
/* 976 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (958) */
/* 978 */	NdrFcShort( 0x0 ),	/* Offset= 0 (978) */
/* 980 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 982 */	0x38,		/* FC_ALIGNM4 */
			0x8,		/* FC_LONG */
/* 984 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 986 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffc65 ),	/* Offset= -923 (64) */
			0x5b,		/* FC_END */
/* 990 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 992 */	NdrFcShort( 0x1 ),	/* 1 */
/* 994 */	NdrFcShort( 0x4 ),	/* 4 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0xfffffc52 ),	/* Offset= -942 (56) */
/* 1000 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 1002 */	NdrFcShort( 0xa ),	/* Offset= 10 (1012) */
/* 1004 */	
			0x13, 0x10,	/* FC_OP */
/* 1006 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1008) */
/* 1008 */	
			0x13, 0x0,	/* FC_OP */
/* 1010 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (972) */
/* 1012 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1014 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1016 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (1004) */
/* 1022 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1024 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1026 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1028 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1030 */	
			0x12, 0x0,	/* FC_UP */
/* 1032 */	NdrFcShort( 0xfffffc10 ),	/* Offset= -1008 (24) */
/* 1034 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1036 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1038 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1040 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1042 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1030) */

			0x0
        }
    };

const CInterfaceProxyVtbl * _cticables2_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ITypesProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ICablesProxyVtbl,
    0
};

const CInterfaceStubVtbl * _cticables2_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ITypesStubVtbl,
    ( CInterfaceStubVtbl *) &_ICablesStubVtbl,
    0
};

PCInterfaceName const _cticables2_InterfaceNamesList[] = 
{
    "ITypes",
    "ICables",
    0
};

const IID *  _cticables2_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _cticables2_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _cticables2, pIID, n)

int __stdcall _cticables2_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _cticables2, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _cticables2, 2, *pIndex )
    
}

const ExtendedProxyFileInfo cticables2_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _cticables2_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _cticables2_StubVtblList,
    (const PCInterfaceName * ) & _cticables2_InterfaceNamesList,
    (const IID ** ) & _cticables2_BaseIIDList,
    & _cticables2_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
