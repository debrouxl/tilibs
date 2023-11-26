/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include "tifiles.h"
#include "gettext.h"
#include "logging.h"
#include "internal.h"
#include "typesxx.h"
#include "error.h"
#include "rwfile.h"


/********************************/
/* Calculator independent types */
/********************************/

/**
 * tifiles_vartype2string:
 * @model: a calculator model.
 * @data: a type ID.
 *
 * Returns the type of variable (REAL, EQU, PRGM, ...).
 *
 * Return value: a string like "REAL".
 **/
const char *TICALL tifiles_vartype2string(CalcModel model, uint8_t data)
{
  switch (model) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_byte2type(TI73_CONST, TI73_MAXTYPES, data);
  case CALC_TI82:
    return tixx_byte2type(TI82_CONST, TI82_MAXTYPES, data);
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_byte2type(TI83_CONST, TI83_MAXTYPES, data);
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_byte2type(data);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_byte2type(data);
  case CALC_TI82A_USB:
    return ti82a_byte2type(data);
  case CALC_TI84PT_USB:
    return ti84pt_byte2type(data);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_byte2type(data);
  case CALC_TI83PCE_USB:
    return ti83pce_byte2type(data);
  case CALC_TI84PCE_USB:
    return ti84pce_byte2type(data);
  case CALC_TI82AEP_USB:
    return ti82aep_byte2type(data);
  case CALC_TI85:
    return tixx_byte2type(TI85_CONST, TI85_MAXTYPES, data);
  case CALC_TI86:
    return tixx_byte2type(TI86_CONST, TI86_MAXTYPES, data);
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_byte2type(data);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_byte2type(data);
  case CALC_TI92:
    return ti92_byte2type(data);
  case CALC_TI92P:
    return ti92p_byte2type(data);
  case CALC_V200:
    return v200_byte2type(data);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_byte2type(NSP_CONST, NSP_MAXTYPES, data);
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return "";
  }
}

/**
 * tifiles_string2vartype:
 * @model: a calculator model.
 * @s: a type as string (like "REAL").
 *
 * Returns the type of variable.
 *
 * Return value: a type ID.
 **/
uint8_t TICALL tifiles_string2vartype(CalcModel model, const char *s)
{
  if (s == nullptr)
  {
    tifiles_critical("%s: invalid string !", __FUNCTION__);
    return 0;
  }

  switch (model) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_type2byte(TI73_CONST, TI73_MAXTYPES, s);
  case CALC_TI82:
    return tixx_type2byte(TI82_CONST, TI82_MAXTYPES, s);
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_type2byte(TI83_CONST, TI83_MAXTYPES, s);
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_type2byte(s);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_type2byte(s);
  case CALC_TI82A_USB:
    return ti82a_type2byte(s);
  case CALC_TI84PT_USB:
    return ti84pt_type2byte(s);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_type2byte(s);
  case CALC_TI83PCE_USB:
    return ti83pce_type2byte(s);
  case CALC_TI84PCE_USB:
    return ti84pce_type2byte(s);
  case CALC_TI82AEP_USB:
    return ti82aep_type2byte(s);
  case CALC_TI85:
    return tixx_type2byte(TI85_CONST, TI85_MAXTYPES, s);
  case CALC_TI86:
    return tixx_type2byte(TI86_CONST, TI86_MAXTYPES, s);
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_type2byte(s);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_type2byte(s);
  case CALC_TI92:
    return ti92_type2byte(s);
  case CALC_TI92P:
    return ti92p_type2byte(s);
  case CALC_V200:
    return v200_type2byte(s);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_type2byte(NSP_CONST, NSP_MAXTYPES, s);
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return 0;
  }
}

/**
 * tifiles_vartype2fext:
 * @model: a calculator model.
 * @data: a type ID.
 *
 * Returns the file extension tipcially used to store this kind of variable(REAL, EQU, PRGM, ...).
 *
 * Return value: a string like "REAL".
 **/
const char *TICALL tifiles_vartype2fext(CalcModel model, uint8_t data)
{
  switch (model) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_byte2fext(TI73_CONST, TI73_MAXTYPES, data, "73?");
  case CALC_TI82:
    return tixx_byte2fext(TI82_CONST, TI82_MAXTYPES, data, "82?");
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_byte2fext(TI83_CONST, TI83_MAXTYPES, data, "83?");
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_byte2fext(data);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_byte2fext(data);
  case CALC_TI82A_USB:
    return ti82a_byte2fext(data);
  case CALC_TI84PT_USB:
    return ti84pt_byte2fext(data);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_byte2fext(data);
  case CALC_TI83PCE_USB:
    return ti83pce_byte2fext(data);
  case CALC_TI84PCE_USB:
    return ti84pce_byte2fext(data);
  case CALC_TI82AEP_USB:
    return ti82aep_byte2fext(data);
  case CALC_TI85:
    return tixx_byte2fext(TI85_CONST, TI85_MAXTYPES, data, "85?");
  case CALC_TI86:
    return tixx_byte2fext(TI86_CONST, TI86_MAXTYPES, data, "86?");
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_byte2fext(data);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_byte2fext(data);
  case CALC_TI92:
    return ti92_byte2fext(data);
  case CALC_TI92P:
    return ti92p_byte2fext(data);
  case CALC_V200:
    return v200_byte2fext(data);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_byte2fext(NSP_CONST, NSP_MAXTYPES, data, "tn?");
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return "";
  }
}

/**
 * tifiles_fext2vartype:
 * @model: a calculator model.
 * @s: a file extension as string (like 89p).
 *
 * Returns the type ID of variable (REAL, EQU, PRGM, ...).
 *
 * Return value: a string like "PRGM".
 **/
uint8_t TICALL tifiles_fext2vartype(CalcModel model, const char *s)
{
  if (s == nullptr)
  {
    tifiles_critical("%s: invalid string !", __FUNCTION__);
    return 0;
  }

  switch (model) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_fext2byte(TI73_CONST, TI73_MAXTYPES, s);
  case CALC_TI82:
    return tixx_fext2byte(TI82_CONST, TI82_MAXTYPES, s);
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_fext2byte(TI83_CONST, TI83_MAXTYPES, s);
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_fext2byte(s);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_fext2byte(s);
  case CALC_TI82A_USB:
    return ti82a_fext2byte(s);
  case CALC_TI84PT_USB:
    return ti84pt_fext2byte(s);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_fext2byte(s);
  case CALC_TI83PCE_USB:
    return ti83pce_fext2byte(s);
  case CALC_TI84PCE_USB:
    return ti84pce_fext2byte(s);
  case CALC_TI82AEP_USB:
    return ti82aep_fext2byte(s);
  case CALC_TI85:
    return tixx_fext2byte(TI85_CONST, TI85_MAXTYPES, s);
  case CALC_TI86:
    return tixx_fext2byte(TI86_CONST, TI86_MAXTYPES, s);
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_fext2byte(s);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_fext2byte(s);
  case CALC_TI92:
    return ti92_fext2byte(s);
  case CALC_TI92P:
    return ti92p_fext2byte(s);
  case CALC_V200:
    return v200_fext2byte(s);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_fext2byte(NSP_CONST, NSP_MAXTYPES, s);
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return 0;
  }
}

/**
 * tifiles_vartype2type:
 * @model: a calculator model.
 * @id: a vartype ID.
 *
 * Returns the type ID of variable as string ("Real", "Program", ...).
 * The function is localized.
 *
 * Return value: a string like "Assembly Program".
 **/
const char *TICALL tifiles_vartype2type(CalcModel model, uint8_t vartype)
{
  switch (model)
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_byte2desc(TI73_CONST, TI73_MAXTYPES, vartype);
  case CALC_TI82:
    return tixx_byte2desc(TI82_CONST, TI82_MAXTYPES, vartype);
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_byte2desc(TI83_CONST, TI83_MAXTYPES, vartype);
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_byte2desc(vartype);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_byte2desc(vartype);
  case CALC_TI82A_USB:
    return ti82a_byte2desc(vartype);
  case CALC_TI84PT_USB:
    return ti84pt_byte2desc(vartype);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_byte2desc(vartype);
  case CALC_TI83PCE_USB:
    return ti83pce_byte2desc(vartype);
  case CALC_TI84PCE_USB:
    return ti84pce_byte2desc(vartype);
  case CALC_TI82AEP_USB:
    return ti82aep_byte2desc(vartype);
  case CALC_TI85:
    return tixx_byte2desc(TI85_CONST, TI85_MAXTYPES, vartype);
  case CALC_TI86:
    return tixx_byte2desc(TI86_CONST, TI86_MAXTYPES, vartype);
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_byte2desc(vartype);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_byte2desc(vartype);
  case CALC_TI92:
    return ti92_byte2desc(vartype);
  case CALC_TI92P:
    return ti92p_byte2desc(vartype);
  case CALC_V200:
    return v200_byte2desc(vartype);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_byte2desc(NSP_CONST, NSP_MAXTYPES, vartype);
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return "";
  }
}

/**
 * tifiles_vartype2icon:
 * @model: a calculator model.
 * @id: a vartype ID.
 *
 * Returns the type ID of variable as string ("Real", "Program", ...).
 * Same as #tifiles_vartype2type but un-localized.
 *
 * Return value: a string like "Assembly Program".
 **/
const char *TICALL tifiles_vartype2icon(CalcModel model, uint8_t vartype)
{
  switch (model) 
  {
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return tixx_byte2icon(TI73_CONST, TI73_MAXTYPES, vartype);
  case CALC_TI82:
    return tixx_byte2icon(TI82_CONST, TI82_MAXTYPES, vartype);
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return tixx_byte2icon(TI83_CONST, TI83_MAXTYPES, vartype);
  case CALC_TI83P:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return ti83p_byte2icon(vartype);
  case CALC_TI84P:
  case CALC_TI84P_USB:
    return ti84p_byte2icon(vartype);
  case CALC_TI82A_USB:
    return ti82a_byte2icon(vartype);
  case CALC_TI84PT_USB:
    return ti84pt_byte2icon(vartype);
  case CALC_TI84PC:
  case CALC_TI84PC_USB:
    return ti84pc_byte2icon(vartype);
  case CALC_TI83PCE_USB:
    return ti83pce_byte2icon(vartype);
  case CALC_TI84PCE_USB:
    return ti84pce_byte2icon(vartype);
  case CALC_TI82AEP_USB:
    return ti82aep_byte2icon(vartype);
  case CALC_TI85:
    return tixx_byte2icon(TI85_CONST, TI85_MAXTYPES, vartype);
  case CALC_TI86:
    return tixx_byte2icon(TI86_CONST, TI86_MAXTYPES, vartype);
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return ti89_byte2icon(vartype);
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return ti89t_byte2icon(vartype);
  case CALC_TI92:
    return ti92_byte2icon(vartype);
  case CALC_TI92P:
    return ti92p_byte2icon(vartype);
  case CALC_V200:
    return v200_byte2icon(vartype);
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return tixx_byte2icon(NSP_CONST, NSP_MAXTYPES, vartype);
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    return "";
  }
}

/****************/
/* Global types */
/****************/

/**
 * tifiles_folder_type
 * @model: a calculator model in #CalcModel enumeration.
 *
 * Returns the variable type ID used for encoding folders.
 *
 * Return value: a type ID.
 **/
uint8_t TICALL tifiles_folder_type(CalcModel model)
{
  switch (model) 
  {
  case CALC_NONE:
    return -1;
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return TI73_DIR;
  case CALC_TI80:
    return -1;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
    return TI83_DIR;
  case CALC_TI83P:
  case CALC_TI84P:
  case CALC_TI84PC:
  case CALC_TI84P_USB:
  case CALC_TI84PC_USB:
  case CALC_TI83PCE_USB:
  case CALC_TI84PCE_USB:
  case CALC_TI82A_USB:
  case CALC_TI84PT_USB:
  case CALC_TI82AEP_USB:
  case CALC_CBL2: // Dubious
  case CALC_CBR2: // Dubious
  case CALC_LABPRO: // Dubious
    return TI83p_DIR;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return TI86_DIR;
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return TI89_DIR;
  case CALC_TI92:
    return TI92_DIR;
  case CALC_TI92P:
    return TI92p_DIR;
  case CALC_V200:
    return V200_DIR;
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
    return NSP_DIR;
  case CALC_TIPRESENTER:
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    break;
  }

  return -1;
}

/**
 * tifiles_flash_type
 * @model: a calculator model in #CalcModel enumeration.
 *
 * Returns the variable type ID used for encoding FLASH apps.
 *
 * Return value: a type ID.
 **/
uint8_t TICALL tifiles_flash_type(CalcModel model)
{
  switch (model) 
  {
  case CALC_NONE:
    return -1;
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return TI73_APPL;
  case CALC_TI80:
    return -1;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return -1;
  case CALC_TI83P:
  case CALC_TI84P:
  case CALC_TI84PC:
  case CALC_TI84P_USB:
  case CALC_TI84PC_USB:
  case CALC_TI83PCE_USB:
  case CALC_TI84PCE_USB:
    return TI83p_APPL;
  case CALC_TI82A_USB:
  case CALC_TI84PT_USB:
  case CALC_TI82AEP_USB:
    return -1;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return -1;
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return TI89_APPL;
  case CALC_TI92:
    return -1;
  case CALC_TI92P:
    return TI92p_APPL;
  case CALC_V200:
    return V200_APPL;
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
  case CALC_TIPRESENTER:
    return -1;
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    break;
  }

  return -1;
}

/**
 * tifiles_idlist_type
 * @model: a calculator model in #CalcModel enumeration.
 *
 * Returns the variable type ID used for encoding IDLIST variable.
 *
 * Return value: a type ID.
 **/
uint8_t TICALL tifiles_idlist_type(CalcModel model)
{
  switch (model) 
  {
  case CALC_NONE:
    return -1;
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return TI73_IDLIST;
  case CALC_TI80:
    return -1;
  case CALC_TI82:
    return -1;
  case CALC_TI83:
  case CALC_CBL:
  case CALC_CBR:
  case CALC_CBL2:
  case CALC_CBR2:
  case CALC_LABPRO:
    return -1;
  case CALC_TI83P:
  case CALC_TI84P:
  case CALC_TI84PC:
  case CALC_TI84P_USB:
  case CALC_TI84PC_USB:
  case CALC_TI83PCE_USB:
  case CALC_TI84PCE_USB:
  case CALC_TI82A_USB:
  case CALC_TI84PT_USB:
  case CALC_TI82AEP_USB:
    return TI83p_IDLIST;
  case CALC_TI85:
    return -1;
  case CALC_TI86:
    return -1;
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return TI89_IDLIST;
  case CALC_TI92:
    return -1;
  case CALC_TI92P:
    return TI92p_IDLIST;
  case CALC_V200:
    return V200_IDLIST;
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
  case CALC_TIPRESENTER:
    return -1;
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    break;
  }

  return -1;
}

/*****************/
/* Miscellaneous */
/*****************/

/**
 * tifiles_calctype2signature:
 * @model: a calculator model.
 *
 * Returns the signature used at the top of a TI file depending on the
 * calculator model.
 *
 * Return value: a string like "**TI89**".
 **/
const char *TICALL tifiles_calctype2signature(CalcModel model)
{
  switch (model) 
  {
  case CALC_NONE:
    return "**TI??**";
#ifndef DISABLE_TI8X
  case CALC_TI73:
    return "**TI73**";
  case CALC_TI82:
    return "**TI82**";
  case CALC_TI83:
  case CALC_CBL: // Dubious
  case CALC_CBR: // Dubious
    return "**TI83**";
  case CALC_TI83P:
  case CALC_TI84P:
  case CALC_TI84PC:
  case CALC_TI84P_USB:
  case CALC_TI84PC_USB:
  case CALC_TI83PCE_USB:
  case CALC_TI84PCE_USB:
  case CALC_TI82A_USB:
  case CALC_TI84PT_USB:
  case CALC_TI82AEP_USB:
  case CALC_CBL2: // Dubious
  case CALC_CBR2: // Dubious
  case CALC_LABPRO: // Dubious
    return "**TI83F*";
  case CALC_TI85:
    return "**TI85**";
  case CALC_TI86:
    return "**TI86**";
#endif
#ifndef DISABLE_TI9X
  case CALC_TI89:
    return "**TI89**";
  case CALC_TI89T:
  case CALC_TI89T_USB:
    return "**TI89**";
  case CALC_TI92:
    return "**TI92**";
  case CALC_TI92P:
  case CALC_V200:
    return "**TI92P*";
#endif
  case CALC_NSPIRE:
  case CALC_NSPIRE_CRADLE:
  case CALC_NSPIRE_CLICKPAD:
  case CALC_NSPIRE_CLICKPAD_CAS:
  case CALC_NSPIRE_TOUCHPAD:
  case CALC_NSPIRE_TOUCHPAD_CAS:
  case CALC_NSPIRE_CX:
  case CALC_NSPIRE_CX_CAS:
  case CALC_NSPIRE_CMC:
  case CALC_NSPIRE_CMC_CAS:
  case CALC_NSPIRE_CXII:
  case CALC_NSPIRE_CXII_CAS:
  case CALC_NSPIRE_CXIIT:
  case CALC_NSPIRE_CXIIT_CAS:
  case CALC_TIPRESENTER:
    return "";
  default:
    tifiles_critical("%s: invalid model argument.", __FUNCTION__);
    break;
  }

  return nullptr;
}

/**
 * tifiles_signature2calctype:
 * @s: a TI file signature like "**TI89**".
 *
 * Returns the calculator model contained in the signature.
 *
 * Return value: a calculator model.
 **/
CalcModel TICALL tifiles_signature2calctype(const char *s)
{
  if (s != nullptr)
  {
    if (!g_ascii_strcasecmp(s, "**TI73**"))
      return CALC_TI73;
    else if (!g_ascii_strcasecmp(s, "**TI82**"))
      return CALC_TI82;
    else if (!g_ascii_strcasecmp(s, "**TI83**"))
      return CALC_TI83;
    else if (!g_ascii_strcasecmp(s, "**TI83F*"))
      return CALC_TI83P;
    else if (!g_ascii_strcasecmp(s, "**TI85**"))
      return CALC_TI85;
    else if (!g_ascii_strcasecmp(s, "**TI86**"))
      return CALC_TI86;
    else if (!g_ascii_strcasecmp(s, "**TI89**"))
      return CALC_TI89;
    else if (!g_ascii_strcasecmp(s, "**TI92**"))
      return CALC_TI92;
    else if (!g_ascii_strcasecmp(s, "**TI92P*"))
      return CALC_TI92P;
    else if (!g_ascii_strcasecmp(s, "**V200**"))
      return CALC_V200;
    else if (!g_ascii_strcasecmp(s, "**TICBL*"))
      return CALC_CBL2;
  }
  tifiles_critical("%s: invalid signature.", __FUNCTION__);
  return CALC_NONE;
}
