// Cables.h : Declaration of the CCables

#ifndef __CABLES_H_
#define __CABLES_H_

#include <oleauto.h>
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCables
class ATL_NO_VTABLE CCables : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCables, &CLSID_Cables>,
	public IDispatchImpl<ICables, &IID_ICables, &LIBID_CTICABLES2Lib>
{
public:
	CCables()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CABLES)
DECLARE_NOT_AGGREGATABLE(CCables)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCables)
	COM_INTERFACE_ENTRY(ICables)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICables
public:
	STDMETHOD(ErrorGet)(/*[in]*/ long code, /*[out]*/ BSTR *message, /*[out, retval]*/ long *ret);
	STDMETHOD(CableGet)(/*[in]*/ long handle, /*[out]*/ unsigned char *data, /*[out, retval]*/ long *ret);
	STDMETHOD(CablePut)(/*[in]*/ long handle, /*[in]*/ unsigned char data, /*[out, retval]*/ long *ret);
	STDMETHOD(ProgressGet)(/*[in]*/ long handle, /*[out]*/ long *count, /*[out]*/ long *msec, /*[out]*/ float *rate, /*[out, retval]*/ long *ret);
	STDMETHOD(ProgressReset)(/*[in]*/ long handle, /*[out, retval]*/ long *ret);
	STDMETHOD(CableGetD1)(/*[in]*/ long handle, /*[out, retval]*/ BOOL *state);
	STDMETHOD(CableGetD0)(/*[in]*/ long handle, /*[out, retval]*/ BOOL *state);
	STDMETHOD(CableSetD1)(/*[in]*/ long handle, /*[in]*/ BOOL state);
	STDMETHOD(CableSetD0)(/*[in]*/ long handle, /*[in]*/ BOOL state);
	STDMETHOD(CableCheck)(/*[in]*/ long handle, /*[out]*/ short *status, /*[out, retval]*/ long *ret);
	STDMETHOD(CableRecv)(/*[in]*/ long handle, /*[out]*/ SAFEARRAY __RPC_FAR * __RPC_FAR *data, /*[in]*/ long count, /*[out, retval]*/ long *ret);
	STDMETHOD(CableSend)(/*[in]*/ long handle, /*[in]*/ SAFEARRAY __RPC_FAR * __RPC_FAR *data, /*[in]*/ long count, /*[out, retval]*/ long *ret);
	STDMETHOD(CableProbe)(/*[in]*/ long handle, /*[out]*/ BOOL *result, /*[out, retval]*/ long *ret);
	STDMETHOD(CableReset)(/*[in]*/ long handle, /*[out, retval]*/ long *ret);
	STDMETHOD(CableClose)(/*[in]*/ long handle, /*[out, retval]*/ long *ret);
	STDMETHOD(CableOpen)(/*[in]*/ long handle, /*[out, retval]*/ long *ret);
	STDMETHOD(HandleShow)(/*[in]*/ long handle);
	STDMETHOD(PortGet)(/*[in]*/ long handle, /*[out, retval]*/ ICablePort *port);
	STDMETHOD(ModelGet)(/*[in]*/ long handle, /*[out, retval]*/ ICableModel *model);
	STDMETHOD(OptionsSetDelay)(/*[in]*/ long handle, /*[in]*/ short delay, /*[out, retval]*/ short *old);
	STDMETHOD(OptionsSetTimeout)(/*[in]*/ long handle, /*[in]*/ short timeout, /*[out, retval]*/ short *old);
	STDMETHOD(HandleDel)(/*[in]*/ long handle);
	STDMETHOD(HandleNew)(/*[in]*/ ICableModel model, /*[in]*/ ICablePort port, /*[out, retval]*/ long *handle);
	STDMETHOD(LibraryExit)(/*[out, retval]*/ long *Cnt);
	STDMETHOD(LibraryInit)(/*[out, retval]*/ long *Cnt);
	STDMETHOD(get_ErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(VersionGet)(/*[out, retval]*/ BSTR *Version);

private:
	long	errnum;
};

#endif //__CABLES_H_
