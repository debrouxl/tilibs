// Types.cpp : Implementation of CTypes
#include "stdafx.h"
#include "Cticables2.h"
#include "Types.h"

#include <ticables.h>
#include "helpers.h"

/////////////////////////////////////////////////////////////////////////////
// CTypes


STDMETHODIMP CTypes::ModelToString(ICableModel model, BSTR *ret)
{
	// TODO: Add your implementation code here
	const char *str = ticables_model_to_string((CableModel)model);

	*ret = SysAllocString(CSTR2BSTR(str));
	if(*ret == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

STDMETHODIMP CTypes::StringToModel(BSTR model, ICableModel *ret)
{
	// TODO: Add your implementation code here
	*ret = (ICableModel)ticables_string_to_model(BSTR2CSTR(model));

	return S_OK;
}

STDMETHODIMP CTypes::PortToString(ICablePort port, BSTR *ret)
{
	// TODO: Add your implementation code here
	const char *str = ticables_port_to_string((CablePort)port);

	*ret = SysAllocString(CSTR2BSTR(str));
	if(*ret == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

STDMETHODIMP CTypes::StringToPort(BSTR port, ICablePort *ret)
{
	// TODO: Add your implementation code here
	*ret = (ICablePort)ticables_string_to_port(BSTR2CSTR(port));

	return S_OK;
}
