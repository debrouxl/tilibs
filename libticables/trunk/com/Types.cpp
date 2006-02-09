/* $Id$ */

/*  cticables2 - COM wrapper for ticables2 library, a part of the TiLP project
 *  Copyright (C) 2006 Romain Liévin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

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
