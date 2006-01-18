// Types.h : Declaration of the CTypes

#ifndef __TYPES_H_
#define __TYPES_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTypes
class ATL_NO_VTABLE CTypes : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTypes, &CLSID_Types>,
	public IDispatchImpl<ITypes, &IID_ITypes, &LIBID_CTICABLES2Lib>
{
public:
	CTypes()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TYPES)
DECLARE_NOT_AGGREGATABLE(CTypes)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTypes)
	COM_INTERFACE_ENTRY(ITypes)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ITypes
public:
	STDMETHOD(StringToPort)(/*[in]*/ BSTR port, /*[out, retval]*/ ICablePort *ret);
	STDMETHOD(PortToString)(/*[in]*/ ICablePort port, /*[out, retval]*/ BSTR *ret);
	STDMETHOD(StringToModel)(/*[in]*/ BSTR model, /*[out, retval]*/ ICableModel *ret);
	STDMETHOD(ModelToString)(/*[in]*/ ICableModel model, /*[out, retval]*/ BSTR *ret);
};

#endif //__TYPES_H_
