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
