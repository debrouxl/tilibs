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