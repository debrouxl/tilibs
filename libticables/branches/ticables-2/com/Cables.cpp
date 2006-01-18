// Cables.cpp : Implementation of CCables
#include "stdafx.h"
#include "Cticables2.h"
#include "Cables.h"

#include <ticables.h>
#include "helpers.h"

/////////////////////////////////////////////////////////////////////////////

#define MAX_DESCRIPTORS	16							/* Maximum number of handles */

CableHandle* handle_list[MAX_DESCRIPTORS] = { 0 };	/* Table of handles */

#define CD_DEREF(cd)	(handle_list[cd])

static int find_free_spot(void)
{
	CableHandle **p;
	int i;

	for(p = handle_list, i = 0; (*p != NULL) && (i < MAX_DESCRIPTORS); p++, i++);

	if(i == MAX_DESCRIPTORS)
		return -1;

	return i;
}

/////////////////////////////////////////////////////////////////////////////
// CCables

STDMETHODIMP CCables::get_ErrorCode(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = errnum;

	return S_OK;
}

STDMETHODIMP CCables::LibraryInit(long *ret)
{
	// TODO: Add your implementation code here
	int cnt = ticables_library_init();
	*ret = cnt;

	return S_OK;
}

STDMETHODIMP CCables::LibraryExit(long *ret)
{
	// TODO: Add your implementation code here
	int cnt = ticables_library_exit();
	*ret = cnt;

	return S_OK;
}

STDMETHODIMP CCables::VersionGet(BSTR *version)
{
	// TODO: Add your implementation code here
	const char *str = ticables_version_get();
	*version = SysAllocString(CSTR2BSTR(str));
	if(*version == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

STDMETHODIMP CCables::HandleNew(ICableModel model, ICablePort port, long *handle)
{
	// TODO: Add your implementation code here
	int cd;
	CableHandle* ch;
	char str[1024];

	ch = ticables_handle_new((CableModel)model, (CablePort)port);

	if(ch == NULL)
		return S_FALSE;

	cd = find_free_spot();
	if(cd == -1)
		return S_FALSE;

	CD_DEREF(cd) = ch;
	*handle = cd;

	sprintf(str, "%p %i", ch, cd);
	//MessageBox(NULL, "Debug", str, MB_OK);

	return S_OK;
}

STDMETHODIMP CCables::HandleDel(long handle)
{
	int cd = handle;

	// TODO: Add your implementation code here
	if(cd < 0 || cd >= MAX_DESCRIPTORS)
		return S_FALSE;
	
	ticables_handle_del(CD_DEREF(cd));

	return S_OK;
}

STDMETHODIMP CCables::OptionsSetTimeout(long handle, short timeout, short *old)
{
	// TODO: Add your implementation code here
	*old = ticables_options_set_timeout(CD_DEREF(handle), timeout);

	return S_OK;
}

STDMETHODIMP CCables::OptionsSetDelay(long handle, short delay, short *old)
{
	// TODO: Add your implementation code here
	*old = ticables_options_set_delay(CD_DEREF(handle), delay);

	return S_OK;
}

STDMETHODIMP CCables::ModelGet(long handle, ICableModel *model)
{
	// TODO: Add your implementation code here
	*model = (ICableModel)ticables_get_model(CD_DEREF(handle));

	return S_OK;
}

STDMETHODIMP CCables::PortGet(long handle, ICablePort *port)
{
	// TODO: Add your implementation code here
	*port = (ICablePort)ticables_get_port(CD_DEREF(handle));

	return S_OK;
}

STDMETHODIMP CCables::HandleShow(long handle)
{
	// TODO: Add your implementation code here
	ticables_handle_show(CD_DEREF(handle));

	return S_OK;
}

STDMETHODIMP CCables::CableOpen(long handle)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_open(CD_DEREF(handle));

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableClose(long handle)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_close(CD_DEREF(handle));

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableReset(long handle)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_reset(CD_DEREF(handle));

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableProbe(long handle, BOOL *result)
{
	int res;

	// TODO: Add your implementation code here
	errnum = ticables_cable_probe(CD_DEREF(handle), &res);
	*result = res ? TRUE : FALSE;

	return S_OK;
}

STDMETHODIMP CCables::CableSend(long handle, SAFEARRAY __RPC_FAR * data, long count)
{
	unsigned char buf[65540];
	long index[1]; index[0]=0;
	int i;
	
	// TODO: Add your implementation code here
	for(i = 0; i < count; i++)
	{
		SafeArrayGetElement(data, index, &buf[i]);
		index[0]++;
	}

	errnum = ticables_cable_send(CD_DEREF(handle), buf, count);

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableRecv(long handle, SAFEARRAY __RPC_FAR * __RPC_FAR *data, long count)
{
	unsigned char buf[65540];
	long index[1]; index[0]=0;
	int i;

	// TODO: Add your implementation code here
	errnum = ticables_cable_recv(CD_DEREF(handle), buf, count);

	// Allocation d'un tableau SAFEARRAY compatible OLE
	*data = SafeArrayCreateVector(VT_I1, 0, count);
	if(*data == NULL)
		return E_OUTOFMEMORY;

	for(i = 0; i < count; i++)
	{
		SafeArrayPutElement(*data, index, &data[i]);
		index[0]++;
	}

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableCheck(long handle, short *status)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_check(CD_DEREF(handle), (CableStatus *)status);

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableSetD0(long handle, BOOL state)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_set_d0(CD_DEREF(handle), state);

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableSetD1(long handle, BOOL state)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_set_d1(CD_DEREF(handle), state);

	return errnum ? S_FALSE : S_OK;
}


STDMETHODIMP CCables::CableGetD0(long handle, BOOL *state)
{
	// TODO: Add your implementation code here
	*state = ticables_cable_get_d0(CD_DEREF(handle));

	return S_OK;
}

STDMETHODIMP CCables::CableGetD1(long handle, BOOL *state)
{
	// TODO: Add your implementation code here
	*state = ticables_cable_get_d1(CD_DEREF(handle));

	return S_OK;
}

STDMETHODIMP CCables::ProgressReset(long handle)
{
	// TODO: Add your implementation code here
	errnum = ticables_progress_reset(CD_DEREF(handle));

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::ProgressGet(long handle, long *count, long *msec, float *rate)
{
	// TODO: Add your implementation code here
	errnum = ticables_progress_get(CD_DEREF(handle), (int *)count, (int *)msec, rate);

	return errnum ? S_FALSE : S_OK;
}


STDMETHODIMP CCables::CablePut(long handle, unsigned char data)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_put(CD_DEREF(handle), data);

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::CableGet(long handle, unsigned char *data)
{
	// TODO: Add your implementation code here
	errnum = ticables_cable_get(CD_DEREF(handle), data);

	return errnum ? S_FALSE : S_OK;
}

STDMETHODIMP CCables::ErrorGet(long code, BSTR *message, long *ret)
{
	// TODO: Add your implementation code here
	char *msg;

	*ret = ticables_error_get(code, &msg);
	*message = SysAllocString(CSTR2BSTR(msg));
	free(msg);

	if(*message == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

