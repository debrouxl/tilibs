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

// Helpers.cpp : convenient functions
#include "stdafx.h"

#include <wchar.h>

//* BSTR2CSTR() ******* USAGE INTERNE ***************************************
//*
//* Descriptif : Methode outil pour la conversion BSTR -> chaine C
//*
//* Paramètres en entrée : 
//*   bstr_sting : Chaine d'entrée de type BSTR
//*
//* Paramètres en sortie : aucun
//*
//* Paramètre de retour : pointeur char* sur chaine C
//*
//***************************************************************************

char* BSTR2CSTR(BSTR bstr_string)
{	
	static char buffer[512];

	wcsrtombs(buffer,(const unsigned short **)(&bstr_string),sizeof(buffer),0);
	return buffer;
}

//* CSTR2BSTR() ******* USAGE INTERNE ***************************************
//*
//* Descriptif : Methode outil pour la conversion Chaine C -> BSTR 
//*
//* Paramètres en entrée : 
//*   cstr_sting : Chaine d'entrée de type C
//*
//* Paramètres en sortie : aucun
//*
//* Paramètre de retour : pointeur wchar_t* sur chaine de type OLE
//*
//***************************************************************************

wchar_t * CSTR2BSTR(const char* cstr_string)
{	
	static wchar_t buffer[512];

	mbsrtowcs(buffer,&cstr_string,sizeof(buffer),0);
	return buffer;
}
