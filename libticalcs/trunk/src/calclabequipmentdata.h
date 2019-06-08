/* Hey EMACS -*- linux-c -*- */

/*  libticalcs2 - hand-helds support library, a part of the TILP project
 *  Copyright (C) 2019  Lionel Debroux
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

// /!\ NOTE: for this file, backwards compatibility will not necessarily be maintained as strongly as it is for ticalcs.h !

#ifndef __CALCLABEQUIPMENTDATA__
#define __CALCLABEQUIPMENTDATA__

#include "ticalcs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Fill a lab equipment data structure in one call.
// @lab_equipment_data must be non-NULL.
// @data and @varname are stored verbatim: in the send path, the caller keeps ownership.
static inline void ticalcs_fill_lab_equipment_data(CalcLabEquipmentData * lab_equipment_data, CalcLabEquipmentDataType type, uint16_t size, uint16_t items, const uint8_t * data, const char * varname, uint8_t vartype, uint8_t sensor_number, uint32_t sensor_mask)
{
	if (nullptr != lab_equipment_data)
	{
		lab_equipment_data->type = type;
		lab_equipment_data->size = size;
		lab_equipment_data->items = items;
		lab_equipment_data->sensor_mask = sensor_mask;
		lab_equipment_data->sensor_number = sensor_number;
		lab_equipment_data->data = data;
		lab_equipment_data->vartype = vartype;
		lab_equipment_data->varname = varname;
	}
}

// Convert a "{v1,v2,...}" list string into the raw list layout selected through
// @lab_equipment_data->type (TI-Z80 9-byte elements, TI-85/86 10-byte elements or TI-68k wire
// text). On success, @lab_equipment_data describes the produced raw list.
TIEXPORT3 int TICALL ticalcs_convert_lab_equipment_data_from_string(CalcLabEquipmentData * lab_equipment_data, const char * string_data);

// Convert a raw list into a "{v1,v2,...}" string plus its decoded numeric values as long doubles.
// Dispatches on @lab_equipment_data->type. On success, @raw_values and @out_data are allocated and
// must be released with ticalcs_free_lab_equipment_data_related().
TIEXPORT3 int TICALL ticalcs_convert_lab_equipment_data_to_string(const CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, long double ** raw_values, char ** out_data);

// Convert a raw list between the 9-byte element layout (TI-73/82/83/83+/84+) and the 10-byte one
// (TI-85/86). @out_data->type must be preset to the desired target type and differ from the source
// type; conversion towards the 9-byte layout fails when an exponent falls outside its +/-99 range.
// Everything but @varname is filled in on success; @data must be released by calling
// #ticalcs_calc_free_lab_equipment_data().
TIEXPORT3 int TICALL ticalcs_convert_lab_equipment_data_raw_list_layout(CalcLabEquipmentData * lab_equipment_data, CalcLabEquipmentData * out_data);

#ifdef __cplusplus
}
#endif

#endif
