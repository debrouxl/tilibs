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

/*
	TI File Format handling routines
	Calcs: 73/82/83/83+/84+/85/86
*/

/*
	Thanks to Adrian Mettler <amettler@hmc.edu> for his patch which fixes
	some TI85/86 file issues (padded, not padded).
*/

#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#include <ticonv.h>
#include "tifiles.h"
#include "error.h"
#include "logging.h"
#include "typesxx.h"
#include "files8x.h"
#include "rwfile.h"
#include "intelhex.h"

#ifndef DISABLE_TI8X

/********/
/* Misc */
/********/

static const uint8_t fsignature85[3] = { 0x1A, 0x0C, 0x00 };	//TI85
static const uint8_t fsignature8x[3] = { 0x1A, 0x0A, 0x00 };	//TI73, 82, 83, 86


static int is_ti8586(CalcModel model)
{
	return ((model == CALC_TI85) || (model == CALC_TI86));
}

static int is_ti83p(CalcModel model)
{
	return ((model == CALC_TI83P) || (model == CALC_TI84P) || (model == CALC_TI84P_USB)
	        || (model == CALC_TI84PC) || (model == CALC_TI84PC_USB)
	        || (model == CALC_TI83PCE_USB) || (model == CALC_TI84PCE_USB) || (model == CALC_TI82A_USB) || (model == CALC_TI84PT_USB) || (model == CALC_TI82AEP_USB));
}

static uint16_t compute_backup_sum(BackupContent* content, uint16_t header_size)
{
	uint16_t sum= 0;

	sum += header_size;
	sum += tifiles_checksum((uint8_t *)&(content->data_length1), 2);
	sum += content->type;
	if (header_size >= 12)
	{
		sum += content->version;
	}
	sum += tifiles_checksum((uint8_t *)&(content->data_length2), 2);
	sum += tifiles_checksum((uint8_t *)&(content->data_length3), 2);
	if (content->model != CALC_TI86)
	{
		sum += tifiles_checksum((uint8_t *)&(content->mem_address), 2);
	}
	else
	{
		sum += tifiles_checksum((uint8_t *)&(content->data_length4), 2);
	}

	sum += tifiles_checksum((uint8_t *)&(content->data_length1), 2);
	sum += tifiles_checksum(content->data_part1, content->data_length1);
	sum += tifiles_checksum((uint8_t *)&(content->data_length2), 2);
	sum += tifiles_checksum(content->data_part2, content->data_length2);
	sum += tifiles_checksum((uint8_t *)&(content->data_length3), 2);
	sum += tifiles_checksum(content->data_part3, content->data_length3);
	sum += tifiles_checksum((uint8_t *)&(content->data_length4), 2);
	sum += tifiles_checksum(content->data_part4, content->data_length4);

	return sum;
}

/***********/
/* Reading */
/***********/

/**
 * ti8x_file_read_regular:
 * @filename: name of single/group file to open.
 * @content: where to store the file content.
 *
 * Load the single/group file into a Ti8xRegular structure.
 *
 * Structure content must be freed with #tifiles_content_delete_regular when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_read_regular(const char *filename, Ti8xRegular *content)
{
	FILE *f;
	uint16_t tmp = 0x000B;
	long offset = 0;
	unsigned int i, j;
	int ti83p_flag = 0;
	uint8_t name_length = 8;	// ti85/86 only
	uint16_t data_size, sum = 0;
	long cur_pos;
	uint32_t file_size;
	char signature[9];
	int padded86 = 0;
	char varname[VARNAME_MAX];
	int ret = ERR_FILE_IO;

	if (content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_regular(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrr2;
	}

	f = g_fopen(filename, "rb");
	if (f == NULL) 
	{
		tifiles_warning("Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrr2;
	}

	// Get file size, then rewind.
	if (fseek(f, 0, SEEK_END) < 0) goto tfrr;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrr;
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrr;

	// The TI-Z80 and TI-eZ80 series' members have at best 4 MB of Flash (TODO: modify this code if this no longer holds).
	// Regular / group files larger than that size are highly dubious, files larger than twice that size are insane.
	if (cur_pos >= (8L << 20))
	{
		ret = ERR_INVALID_FILE;
		goto tfrr;
	}
	file_size = (uint32_t)cur_pos;

	if (fread_8_chars(f, signature) < 0) goto tfrr; // Offset 0
	content->model = tifiles_signature2calctype(signature);
	if (content->model == CALC_NONE)
	{
		ret = ERR_INVALID_FILE;
		goto tfrr;
	}
	if (content->model_dst == CALC_NONE)
	{
		content->model_dst = content->model;
	}
	if (fskip(f, 3) < 0) goto tfrr; // Offset 0x8
	if (fread_n_chars(f, 42, content->comment) < 0) goto tfrr; // Offset 0xB
	if (fread_word(f, &data_size) < 0) goto tfrr; // Offset 0x35
	if ((uint32_t)data_size > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrr;
	}

	// search for the number of entries by parsing the whole file
	offset = ftell(f); // Offset 0x37
	if (offset == -1L) goto tfrr;

	for (i = 0;; i++) 
	{
		long current_offset = ftell(f);
		/* We are done finding entries once we reach the end of the data segment
		 * as defined in the header.  This works better than magic numbers, as
		 * as there exist files in the wild with incorrect magic numbers that
		 * transmit correctly with TI's software and with this code.
		 *   Adrian Mettler
		 */
		if (current_offset == -1L) goto tfrr;
		if (current_offset >= offset + data_size)
		{
			break;
		}

		if (fread_word(f, &tmp) < 0) goto tfrr; // Offset N, 0x37 for the first entry
		if (tmp == 0x0D)
		{
			ti83p_flag = !0;		// true TI83+ file (2 extra bytes)
		}

		if (content->model == CALC_TI85)
		{
			// length &  name with no padding
			if (fskip(f, 3) < 0) goto tfrr; // Offset N+2, 0x39 for the first entry
			if (fread_byte(f, &name_length) < 0) goto tfrr; // Offset N+5, 0x3C for the first entry
			if (name_length > 8)
			{
				ret = ERR_INVALID_FILE;
				goto tfrr;
			}
			if (fskip(f, name_length) < 0) goto tfrr; // Offset N+6+name_length, 0x3D for the first entry
		}
		else if (content->model == CALC_TI86)
		{
			/* name may follow one of four conventions: padded with SPC bytes
			 * (most correct, generated by TI's software), padded with NULL bytes,
			 * unpadded (like TI85) or partially padded (garbaged).  
			 * TI's software accepts all four, so we should too.
			 */
			padded86 = tmp < 0x0C ? 0 : !0;	// TI-85 style file

			if (fskip(f, 3) < 0) goto tfrr; // Offset N+2, 0x39 for the first entry
			if (fread_byte(f, &name_length) < 0) goto tfrr; // Offset N+5, 0x3C for the first entry
			if (name_length > 8)
			{
				ret = ERR_INVALID_FILE;
				goto tfrr;
			}
			if (fskip(f, name_length) < 0) goto tfrr; // Offset N+6+name_length, 0x3D for the first entry

			if (padded86)
			{
				if (fskip(f, 8 - name_length) < 0) goto tfrr;
			}
		}
		else if (ti83p_flag)
		{
			if (fskip(f, 13) < 0) goto tfrr; // Offset N+2, 0x39 for the first entry.
		}
		else
		{
			if (fskip(f, 11) < 0) goto tfrr; // Offset N+2, 0x39 for the first entry.
		}
		if (fread_word(f, &tmp) < 0) goto tfrr; // Offset depends on model.
		if (fskip(f, tmp) < 0) goto tfrr;
	}

	if (fseek(f, offset, SEEK_SET) < 0) goto tfrr; // Offset 0x37

	content->num_entries = i;
	content->entries = (VarEntry **)g_malloc0((content->num_entries + 1) * sizeof(VarEntry*));
	if (content->entries == NULL) 
	{
		ret = ERR_MALLOC;
		goto tfrr;
	}

	for (i = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i] = (VarEntry *)g_malloc0(sizeof(VarEntry));
		uint16_t packet_length, entry_size;

		if (fread_word(f, &packet_length) < 0) goto tfrr; // Offset 0x37
		if ((uint32_t)packet_length > file_size)
		{
			ret = ERR_INVALID_FILE;
			goto tfrr;
		}
		if (fread_word(f, &entry_size) < 0) goto tfrr; // Offset 0x39
		if ((uint32_t)entry_size > file_size)
		{
			ret = ERR_INVALID_FILE;
			goto tfrr;
		}
		entry->size = entry_size;
		if (fread_byte(f, &(entry->type)) < 0) goto tfrr; // Offset 0x3B
		if (is_ti8586(content->model))
		{
			if (fread_byte(f, &name_length) < 0) goto tfrr; // Offset 0x3C
		}
		if (fread_n_chars(f, name_length, varname) < 0) goto tfrr; // Offset up to 0x44
		ticonv_varname_from_tifile_sn(content->model_dst, varname, entry->name, sizeof(entry->name), entry->type);
		if ((content->model == CALC_TI86) && padded86)
		{
			for (j = 0; j < 8U - name_length; j++)
			{
				sum += fgetc(f);
			}
		}
		if (ti83p_flag) 
		{
			uint16_t attribute;
			if (fread_word(f, &attribute) < 0) goto tfrr; // Offset 0x44
			// Handle both the files created by TI-Connect and the files created by
			// some broken versions of libtifiles.
			if (attribute == 0x80)
			{
				entry->attr = ATTRB_ARCHIVED;
				entry->version = 0;
			}
			else
			{
				entry->attr = ((attribute & 0x8000) ? ATTRB_ARCHIVED : ATTRB_NONE);
				entry->version = attribute & 0xff;
			}

			// Handle broken 84+CSE Pic files created by older versions of libtifiles.
			if (entry->type == TI84p_PIC && entry->size == 0x55bb && entry->version == 0)
			{
				entry->version = 10;
			}

			sum += MSB(attribute);
			sum += LSB(attribute);
		}
		if (fread_word(f, NULL) < 0) goto tfrr;

		entry->data = (uint8_t *) g_malloc0(entry->size);
		if (entry->data == NULL) 
		{
			ret = ERR_MALLOC;
			goto tfrr;
		}

		if (fread(entry->data, 1, entry->size, f) < entry->size) goto tfrr;

		sum += packet_length;
		sum += tifiles_checksum((uint8_t *)&(entry->size), 2);
		sum += entry->type;
		if (is_ti8586(content->model))
		{
			sum += strlen(entry->name);
		}
		sum += tifiles_checksum((uint8_t *)varname, name_length);
		sum += 0; // see above (file may be padded with garbage)
		sum += tifiles_checksum((uint8_t *)&(entry->size), 2);
		sum += tifiles_checksum(entry->data, entry->size);
	}

	if (fread_word(f, &(content->stored_checksum)) < 0) goto tfrr;
	if (sum != content->stored_checksum)
	{
		tifiles_warning("Checksum mismatch in %s: computed: %0x04X / stored: %0x04X", filename, sum, content->stored_checksum);
	}
	content->checksum = sum;	// sum of all checksums but unused

	fclose(f);
	return 0;

tfrr:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrr2:
	tifiles_content_delete_regular(content);
	return ret;
}

/**
 * ti8x_file_read_backup:
 * @filename: name of backup file to open.
 * @content: where to store the file content.
 *
 * Load the backup file into a Ti8xBackup structure.
 *
 * Structure content must be freed with #tifiles_content_delete_backup when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_read_backup(const char *filename, Ti8xBackup *content)
{
	FILE *f;
	long cur_pos = 0;
	char signature[9];
	uint16_t sum;
	uint16_t file_size, header_size;
	uint8_t extra_header[3] = { 0, 0, 0 };
	int ret = ERR_FILE_IO;

	if (content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_backup(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrb2;
	}

	f = g_fopen(filename, "rb");
	if (f == NULL) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrb2;
	}

	// Get file size, then rewind.
	if (fseek(f, 0, SEEK_END) < 0) goto tfrb;
	cur_pos = ftell(f);
	if (cur_pos < 0) goto tfrb;
	if (fseek(f, 0, SEEK_SET) < 0) goto tfrb;

	// The TI-Z80 backup format cannot handle more than 65535 bytes.
	if (cur_pos >= 65536L)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	file_size = (uint16_t)cur_pos;

	if (fread_8_chars(f, signature) < 0) goto tfrb;
	content->model = tifiles_signature2calctype(signature);
	if (content->model == CALC_NONE)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	if (fskip(f, 3) < 0) goto tfrb;
	if (fread_n_chars(f, 42, content->comment) < 0) goto tfrb;
	if (fread_word(f, NULL) < 0) goto tfrb;

	if (fread_word(f, &header_size) < 0) goto tfrb;
	if (header_size < 9 || header_size > 12)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}

	if (fread_word(f, &(content->data_length1)) < 0) goto tfrb;
	if (content->data_length1 > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	if (fread_byte(f, &(content->type)) < 0) goto tfrb;
	if (fread_word(f, &(content->data_length2)) < 0) goto tfrb;
	if (content->data_length2 > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	if (fread_word(f, &(content->data_length3)) < 0) goto tfrb;
	if (content->data_length3 > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	content->data_length4 = 0;
	if (content->model != CALC_TI86)
	{
		if (fread_word(f, &(content->mem_address)) < 0) goto tfrb;
	}
	else
	{
		if (fread_word(f, &(content->data_length4)) < 0) goto tfrb;
	}
	if (content->data_length4 > file_size)
	{
		ret = ERR_INVALID_FILE;
		goto tfrb;
	}
	if (header_size > 9)
	{
		if (fread(extra_header, 1, header_size - 9, f) < (size_t)header_size - 9) goto tfrb;
	}
	content->version = extra_header[2];

	if (fread_word(f, NULL) < 0) goto tfrb;
	content->data_part1 = (uint8_t *)g_malloc0(content->data_length1);
	if (content->data_part1 == NULL) 
	{
		ret = ERR_MALLOC;
		goto tfrb;
	}
	if (fread(content->data_part1, 1, content->data_length1, f) < content->data_length1) goto tfrb;

	if (fread_word(f, NULL) < 0) goto tfrb;
	content->data_part2 = (uint8_t *)g_malloc0(content->data_length2);
	if (content->data_part2 == NULL) 
	{
		ret = ERR_MALLOC;
		goto tfrb;
	}
	if (fread(content->data_part2, 1, content->data_length2, f) < content->data_length2) goto tfrb;

	if (content->data_length3)	// can be 0000 on TI86
	{
		if (fread_word(f, NULL) < 0) goto tfrb;
		content->data_part3 = (uint8_t *)g_malloc0(content->data_length3);
		if (content->data_part3 == NULL) 
		{
			ret = ERR_MALLOC;
			goto tfrb;
		}
		if (fread(content->data_part3, 1, content->data_length3, f) < content->data_length3) goto tfrb;
	}

	if (content->model == CALC_TI86) 
	{
		if (fread_word(f, NULL) < 0) goto tfrb;
		content->data_part4 = (uint8_t *)g_malloc0(content->data_length4);
		if (content->data_part4 == NULL) 
		{
			ret = ERR_MALLOC;
			goto tfrb;
		}
		if (fread(content->data_part4, 1, content->data_length4, f) < content->data_length4) goto tfrb;
	}
	else
	{
		content->data_length4 = 0;
		content->data_part4 = NULL;
	}

	if (fread_word(f, &(content->stored_checksum)) < 0) goto tfrb;
	sum = compute_backup_sum(content, header_size);
	if (sum != content->stored_checksum)
	{
		tifiles_warning("Checksum mismatch in %s: computed: %0x04X / stored: %0x04X", filename, sum, content->stored_checksum);
	}
	content->checksum = sum;	// sum of all checksums but unused

	fclose(f);
	return 0;

tfrb:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrb2:
	tifiles_content_delete_backup(content);
	return ret;
}

static int check_device_type(uint8_t id)
{
	static const uint8_t types[] = { 0, DEVICE_TYPE_73, DEVICE_TYPE_83P };
	int i;

	for (i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
	{
		if (types[i] == id)
		{
			return i;
		}
	}

	return 0;
}

static int check_data_type(uint8_t id)
{
	static const uint8_t types[] = { 0, TI83p_AMS, TI83p_APPL, TI83p_CERT, TI83p_LICENSE };
	int i;

	for (i = 1; i < (int)(sizeof(types)/sizeof(types[0])); i++)
	{
		if (types[i] == id)
		{
			return i;
		}
	}

	return 0;
}

static int get_native_app_name(const FlashContent *content, char *buffer, size_t buffer_size)
{
	const uint8_t *data, *name;
	uint16_t app_type;
	uint32_t size, n;

	if (content->num_pages > 0)
	{
		data = content->pages[0]->data;
		size = content->pages[0]->size;
	}
	else
	{
		data = content->data_part;
		size = content->data_length;
	}

	if (size >= 6 && (data[0] & 0xf0) == 0x80 && data[1] == 0x0f)
	{
		app_type = (uint16_t)(data[0]) << 8;
		if (!tifiles_cert_field_find(data + 6, size - 6, app_type + 0x40, &name, &n))
		{
			if (n >= buffer_size)
			{
				n = buffer_size - 1;
			}
			memcpy(buffer, name, n);
			buffer[n] = 0;
			return 1;
		}
	}

	return 0;
}

/**
 * ti8x_file_read_flash:
 * @filename: name of flash file to open.
 * @content: where to store the file content.
 *
 * Load the flash file into a #FlashContent structure.
 *
 * Structure content must be freed with #tifiles_content_delete_flash when
 * no longer used. If error occurs, the structure content is released for you.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_read_flash(const char *filename, Ti8xFlash *head)
{
	FILE *f;
	Ti8xFlash *content = head;
	int i;
	char signature[9];
	char varname[9];
	int ret = ERR_FILE_IO;

	if (head == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (!tifiles_file_is_flash(filename))
	{
		ret = ERR_INVALID_FILE;
		goto tfrf2;
	}

	f = g_fopen(filename, "rb");
	if (f == NULL)
	{
		tifiles_info("Unable to open this file: %s", filename);
		ret = ERR_FILE_OPEN;
		goto tfrf2;
	}

	for (content = head;; content = content->next) 
	{
		if (fread_8_chars(f, signature) < 0) goto tfrf;
		content->model = tifiles_file_get_model(filename);
		if (fread_byte(f, &(content->revision_major)) < 0) goto tfrf;
		if (fread_byte(f, &(content->revision_minor)) < 0) goto tfrf;
		if (fread_byte(f, &(content->flags)) < 0) goto tfrf;
		if (fread_byte(f, &(content->object_type)) < 0) goto tfrf;
		if (fread_byte(f, &(content->revision_day)) < 0) goto tfrf;
		if (fread_byte(f, &(content->revision_month)) < 0) goto tfrf;
		if (fread_word(f, &(content->revision_year)) < 0) goto tfrf;
		if (fskip(f, 1) < 0) goto tfrf;
		if (fread_8_chars(f, varname) < 0) goto tfrf;
		if (fskip(f, 23) < 0) goto tfrf;
		if (fread_byte(f, &(content->device_type)) < 0) goto tfrf;
		if (fread_byte(f, &(content->data_type)) < 0) goto tfrf;
		if (fskip(f, 23) < 0) goto tfrf;
		if (fread_byte(f, &(content->hw_id)) < 0) goto tfrf;
		if (fread_long(f, &content->data_length) < 0) goto tfrf;

		if (!check_device_type(content->device_type))
		{
			ret = ERR_INVALID_FILE;
			goto tfrf;
		}
		if (!check_data_type(content->data_type))
		{
			ret = ERR_INVALID_FILE;
			goto tfrf;
		}
		// TODO: modify this code if TI ever makes a TI-eZ80 model with more than 4 MB of Flash memory...
		if (content->data_length > 4U * 1024 * 1024 - 16384)
		{
			// Data length larger than Flash memory size - boot code sector size doesn't look right.
			ret = ERR_INVALID_FILE;
			goto tfrf;
		}

		if (content->data_type == TI83p_CERT || content->data_type == TI83p_LICENSE)
		{
			// get data like TI9X
			content->data_part = (uint8_t *)g_malloc0(content->data_length + 256);
			if (content->data_part == NULL) 
			{
				ret = ERR_MALLOC;
				goto tfrf;
			}

			memset(content->data_part, 0xff, content->data_length + 256);
			if (fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;

			content->next = NULL;
		}
		else if (content->data_type == TI83p_AMS || content->data_type == TI83p_APPL)
		{
			if (content->hw_id == 0)
			{
				// This looks like a TI-Z80-style OS / FlashApp, let's try to parse Intel Hex format.

				// reset/initialize block reader
				hex_block_read(NULL, NULL, NULL, NULL, NULL, NULL);
				content->pages = NULL;

				// TODO: modify this code if TI ever makes a TI-Z80 model with more than 256 Flash pages...
				content->pages = (FlashPage **)g_malloc0((256+1) * sizeof(Ti8xFlashPage *));
				if (content->pages == NULL)
				{
					ret = ERR_MALLOC;
					goto tfrf;
				}

				// read FLASH pages
				content->data_length = 0;
				for (i = 0, ret = 0; !ret; i++)
				{
					uint16_t size;
					uint16_t addr;
					uint16_t page;
					uint8_t flag = 0x80;
					uint8_t data[FLASH_PAGE_SIZE];
					FlashPage* fp = content->pages[i] = (FlashPage *)g_malloc0(sizeof(FlashPage));

					ret = hex_block_read(f, &size, &addr, &flag, data, &page);

					fp->data = (uint8_t *) g_malloc0(FLASH_PAGE_SIZE);
					memset(fp->data, 0xff, FLASH_PAGE_SIZE);
					if (fp->data == NULL)
					{
						ret = ERR_MALLOC;
						goto tfrf;
					}

					fp->addr = addr;
					fp->page = page;
					fp->flag = flag;
					fp->size = size;
					memcpy(fp->data, data, size);

					content->data_length += size;
				}
				content->num_pages = i;
				content->next = NULL;
			}
			else
			{
				// This looks like a TI-68k-style OS / FlashApp, containing flat data.

				content->data_part = (uint8_t *)g_malloc0(content->data_length);
				if (content->data_part == NULL)
				{
					ret = ERR_MALLOC;
					goto tfrf;
				}

				if (fread(content->data_part, 1, content->data_length, f) < content->data_length) goto tfrf;
				if (   (content->data_type == TI83p_AMS && content->data_part[0] != 0x80)
				    || (content->data_type == TI83p_APPL && content->data_part[0] != 0x81))
				{
					ret = ERR_INVALID_FILE;
					goto tfrf;
				}
				content->next = NULL;
			}
		}

		if (content->data_type == TI83p_APPL)
		{
			// Determine the app name from the internal header if possible.
			if (!get_native_app_name(content, varname, sizeof(varname)))
			{
				tifiles_warning("unable to determine app name from header");
			}
		}

		if (content->model_dst == CALC_NONE)
		{
			content->model_dst = content->model;
		}
		ticonv_varname_from_tifile_sn(content->model_dst, varname, content->name, sizeof(content->name), content->data_type);

		// check for end of file
		if (fread_8_chars(f, signature) < 0)
		{
			break;
		}
		if (strcmp(signature, "**TIFL**") || feof(f))
		{
			break;
		}
		if (fseek(f, -8, SEEK_CUR)) goto tfrf;

		content->next = (Ti8xFlash *)g_malloc0(sizeof(Ti8xFlash));
		if (content->next == NULL) 
		{
			ret = ERR_MALLOC;
			goto tfrf;
		}
	}

	fclose(f);
	return 0;

tfrf:	// release on exit
	tifiles_critical("%s: error reading / understanding file %s", __FUNCTION__, filename);
	fclose(f);
tfrf2:
	tifiles_content_delete_flash(content);
	return ret;
}

/***********/
/* Writing */
/***********/

/**
 * ti8x_file_write_regular:
 * @filename: name of single/group file where to write or NULL.
 * @content: the file content to write.
 * @real_filename: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write one (or several) variable(s) into a single (group) file. If filename is set to NULL,
 * the function build a filename from varname and allocates resulting filename in %real_fname.
 * %filename and %real_filename can be NULL but not both !
 *
 * %real_filename must be freed when no longer used.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_write_regular(const char *fname, Ti8xRegular *content, char **real_fname)
{
	FILE *f;
	unsigned int i;
	uint16_t sum = 0;
	char *filename = NULL;
	uint32_t data_length;
	uint16_t packet_length = 0x0B;
	uint8_t name_length = 8;
	uint16_t attr;

	if (content->entries == NULL)
	{
		tifiles_warning("%s: skipping content with NULL content->entries", __FUNCTION__);
		return 0;
	}

	// Compute data length now, so as to be able to bail out immediately if we know the data won't fit.
	for (i = 0, data_length = 0; i < content->num_entries; i++)
	{
		VarEntry *entry = content->entries[i];
		if (entry == NULL)
		{
			tifiles_warning("%s: skipping null content entry %d", __FUNCTION__, i);
			continue;
		}

		if (content->model == CALC_TI82 || content->model == CALC_TI73)
		{
			data_length += entry->size + 15;
		}
		else if (content->model == CALC_TI83)
		{
			data_length += entry->size + 15;
		}
		else if (content->model == CALC_TI85)
		{
			data_length += entry->size + 8 + strlen(entry->name);
		}
		else if (content->model == CALC_TI86)
		{
			data_length += entry->size + 16;
		}
		else if (is_ti83p(content->model))
		{
			data_length += entry->size + 17;
		}
	}
	if (data_length > 65535)
	{
		return ERR_GROUP_SIZE;
	}

	if (fname != NULL)
	{
		filename = g_strdup(fname);
		if (filename == NULL)
		{
			return ERR_MALLOC;
		}
	}
	else
	{
		if (content->entries[0])
		{
			filename = tifiles_build_filename(content->model_dst, content->entries[0]);
		}
		else
		{
			tifiles_warning("%s: asked to build a filename from null content->entries[0], bailing out", __FUNCTION__);
			if (real_fname != NULL)
			{
				*real_fname = NULL;
			}
			return 0;
		}
		if (real_fname != NULL)
		{
			*real_fname = g_strdup(filename);
		}
	}

	f = g_fopen(filename, "wb");
	if (f == NULL) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		g_free(filename);
		return ERR_FILE_OPEN;
	}

	// write header
	if (fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwr;
	if (fwrite(content->model == CALC_TI85 ? fsignature85 : fsignature8x, 1, 3, f) < 3) goto tfwr;
	if (fwrite_n_bytes(f, 42, (uint8_t *)content->comment) < 0) goto tfwr;

	if (fwrite_word(f, (uint16_t) data_length) < 0) goto tfwr;

	// write data section
	for (i = 0, sum = 0; i < content->num_entries; i++) 
	{
		VarEntry *entry = content->entries[i];
		char varname[VARNAME_MAX];

		if (content->model == CALC_TI85)
		{
			packet_length = 4 + strlen(entry->name);	//offset to data length
		}
		else if (content->model == CALC_TI86)
		{
			packet_length = 0x0C;
		}
		else if (is_ti83p(content->model))
		{
			packet_length = 0x0D;
		}
		else
		{
			packet_length = 0x0B;
 		}

		if (fwrite_word(f, packet_length) < 0) goto tfwr;
		if (fwrite_word(f, (uint16_t)entry->size) < 0) goto tfwr;
		if (fwrite_byte(f, entry->type) < 0) goto tfwr;
		memset(varname, 0, sizeof(varname));
		ticonv_varname_to_tifile_sn(content->model_dst, entry->name, varname, sizeof(varname), entry->type);
		if (is_ti8586(content->model)) 
		{
			name_length = strlen(varname);
			if (fwrite_byte(f, (uint8_t)name_length) < 0) goto tfwr;
			if (content->model == CALC_TI85)
			{
				if (fwrite_n_chars(f, name_length, varname) < 0) goto tfwr;
			}
			else
			{
				if (fwrite_n_chars2(f, 8, varname) < 0) goto tfwr; // space padded
			}
		}
		else
		{
			if (fwrite_n_chars(f, 8, varname) < 0) goto tfwr;
		}
		if (is_ti83p(content->model))
		{
			attr = (uint16_t)((entry->attr == ATTRB_ARCHIVED) ? 0x8000 : 0x00) + entry->version;
			if (fwrite_word(f, attr) < 0) goto tfwr;
			sum += MSB(attr);
			sum += LSB(attr);
		}
		if (fwrite_word(f, (uint16_t)entry->size) < 0) goto tfwr;
		if (fwrite(entry->data, 1, entry->size, f) < entry->size) goto tfwr;

		sum += packet_length;
		sum += MSB(entry->size);
		sum += LSB(entry->size);
		sum += entry->type;
		if (is_ti8586(content->model))
		{
			sum += strlen(entry->name);
		}
		sum += tifiles_checksum((uint8_t *)varname, name_length);
		if (content->model == CALC_TI86)
		{
			sum += (8 - name_length) * ' ';
		}
		sum += MSB(entry->size);
		sum += LSB(entry->size);
		sum += tifiles_checksum(entry->data, entry->size);
	}

	//checksum is the sum of all bytes in the data section
	content->checksum = sum;
	if (fwrite_word(f, content->checksum) < 0) goto tfwr;

	g_free(filename);
	fclose(f);
	return 0;

tfwr:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	g_free(filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**
 * ti8x_file_write_backup:
 * @filename: name of backup file where to write.
 * @content: the file content to write.
 *
 * Write content to a backup file.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_write_backup(const char *filename, Ti8xBackup *content)
{
	FILE *f;
	uint16_t header_size;
	uint32_t data_length;

	if (filename == NULL || content == NULL)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	data_length = content->data_length1 + content->data_length2 + content->data_length3 + 17;
	if (content->model == CALC_TI86)
	{
		data_length += content->data_length4;
	}

	if (data_length > 65535)
	{
		return ERR_GROUP_SIZE;
	}

	f = g_fopen(filename, "wb");
	if (f == NULL) 
	{
		tifiles_info( "Unable to open this file: %s", filename);
		return ERR_FILE_OPEN;
	}
	// write header
	if (fwrite_8_chars(f, tifiles_calctype2signature(content->model)) < 0) goto tfwb;
	if (fwrite(content->model == CALC_TI85 ? fsignature85 : fsignature8x, 1, 3, f) < 3) goto tfwb;
	if (fwrite_n_bytes(f, 42, (uint8_t *)content->comment) < 0) goto tfwb;
	if (fwrite_word(f, data_length) < 0) goto tfwb;

	// Use the old-style header for versions 0 to 5 (84+ OS 2.48 and
	// earlier), for compatibility with older versions of libtifiles
	// that assume the header is always 9 bytes.  Backups from newer
	// OSes (2.53 and later) are incompatible in both directions, so a
	// new format is needed for version 6 and above.
	if ((content->version & ~0x20) <= 0x05)
	{
		header_size = 9;
	}
	else
	{
		header_size = 12;
	}

	// write backup header
	if (fwrite_word(f, header_size) < 0) goto tfwb;
	if (fwrite_word(f, content->data_length1) < 0) goto tfwb;
	if (fwrite_byte(f, content->type) < 0) goto tfwb;
	if (fwrite_word(f, content->data_length2) < 0) goto tfwb;
	if (fwrite_word(f, content->data_length3) < 0) goto tfwb;
	if (content->model != CALC_TI86)
	{
		if (fwrite_word(f, content->mem_address) < 0) goto tfwb;
	}
	else
	{
		if (fwrite_word(f, content->data_length4) < 0) goto tfwb;
	}
	if (header_size == 12)
	{
		if (fwrite_word(f, 0) < 0) goto tfwb;
		if (fwrite_byte(f, content->version) < 0) goto tfwb;
	}

	// write data num_entries
	if (fwrite_word(f, content->data_length1) < 0) goto tfwb;
	if (fwrite(content->data_part1, 1, content->data_length1, f) < content->data_length1) goto tfwb;
	if (fwrite_word(f, content->data_length2) < 0) goto tfwb;
	if (fwrite(content->data_part2, 1, content->data_length2, f) < content->data_length2) goto tfwb;
	if (content->data_length3)	// TI86: can be NULL
	{
		if (fwrite_word(f, content->data_length3) < 0) goto tfwb;
	}
	if (fwrite(content->data_part3, 1, content->data_length3, f) < content->data_length3) goto tfwb;
	if (content->model == CALC_TI86)
	{
		if (fwrite_word(f, content->data_length4) < 0) goto tfwb;
		if (fwrite(content->data_part4, 1, content->data_length4, f) < content->data_length4) goto tfwb;
	}

	// checksum = sum of all bytes in bachup headers and data num_entries
	content->checksum = compute_backup_sum(content, header_size);
	if (fwrite_word(f, content->checksum) < 0) goto tfwb;

	fclose(f);
	return 0;

tfwb:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	fclose(f);
	return ERR_FILE_IO;
}

/**
 * ti8x_file_write_flash:
 * @filename: name of flash file where to write.
 * @content: the file content to write.
 * @real_filename: pointer address or NULL. Must be freed if needed when no longer needed.
 *
 * Write content to a flash file (os or app).
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_write_flash(const char *fname, Ti8xFlash *head, char **real_fname)
{
	FILE *f;
	Ti8xFlash *content = head;
	unsigned int i;
	int bytes_written = 0;
	long pos;
	char *filename;
	char varname[VARNAME_MAX];

	if (head == NULL)
	{
		tifiles_critical("%s: head is NULL", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	if (fname)
	{
		filename = g_strdup(fname);
		if (filename == NULL)
		{
			return ERR_MALLOC;
		}
	}
	else
	{
		VarEntry ve;

		for (content = head; content != NULL; content = content->next)
		{
			if (content->data_type == TI83p_AMS || content->data_type == TI83p_APPL)
			{
				break;
			}
		}
		if (content == NULL)
		{
			tifiles_critical("%s: content is NULL", __FUNCTION__);
			return ERR_BAD_FILE;
		}

		strncpy(ve.name, content->name, sizeof(ve.name) - 1);
		ve.name[sizeof(ve.name) - 1] = 0;
		ve.type = content->data_type;

		filename = tifiles_build_filename(content->model_dst, &ve);
		if (real_fname != NULL)
		{
			*real_fname = g_strdup(filename);
		}
	}

	f = g_fopen(filename, "wb");
	if (f == NULL) 
	{
		tifiles_info("Unable to open this file: %s", filename);
		g_free(filename);
		return ERR_FILE_OPEN;
	}

	for (content = head; content != NULL; content = content->next) 
	{
		// header
		if (fwrite_8_chars(f, "**TIFL**") < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_major) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_minor) < 0) goto tfwf;
		if (fwrite_byte(f, content->flags) < 0) goto tfwf;
		if (fwrite_byte(f, content->object_type) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_day) < 0) goto tfwf;
		if (fwrite_byte(f, content->revision_month) < 0) goto tfwf;
		if (fwrite_word(f, content->revision_year) < 0) goto tfwf;

		memset(varname, 0, sizeof(varname));
		ticonv_varname_to_tifile_sn(content->model_dst, content->name, varname, sizeof(varname), content->data_type);
		if (content->data_type == TI83p_APPL)
		{
			// Determine the app name from the internal header if possible.
			get_native_app_name(content, varname, sizeof(varname));
		}
		if (fwrite_byte(f, (uint8_t) strlen(varname)) < 0) goto tfwf;
		if (fwrite_8_chars(f, varname) < 0) goto tfwf;
		for (i = 0; i < 23; i++)
		{
			if (fwrite_byte(f, 0) < 0) goto tfwf;
		}
		if (fwrite_byte(f, content->device_type) < 0) goto tfwf;
		if (fwrite_byte(f, content->data_type) < 0) goto tfwf;
		for (i = 0; i < 23; i++)
		{
			if (fwrite_byte(f, 0) < 0) goto tfwf;
		}
		if (fwrite_byte(f, content->hw_id) < 0) goto tfwf;
		pos = ftell(f);
		if (pos == -1L) goto tfwf;
		if (fwrite_long(f, content->data_length) < 0) goto tfwf;

		// data
		if (content->data_type == TI83p_CERT || content->data_type == TI83p_LICENSE)
		{
			if (fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwf;
		}
		else if (content->data_type == TI83p_AMS || content->data_type == TI83p_APPL)
		{
			// write
			if (content->hw_id == 0)
			{
				// TI-Z80-style OS / FlashApp, let's write Intel Hex format.
				for (i = 0; content->pages != NULL && i < content->num_pages; i++)
				{
					uint32_t app_length, page_length;
					int extra_bytes = 0;

					page_length = content->pages[i]->size;

					if (   content->data_type == TI83p_APPL && i == content->num_pages - 1
					    && content->pages[0]->data[0] == 0x80 && content->pages[0]->data[1] == 0x0f)
					{
						/* Flash app signing programs will usually add some
						   padding to the end of the app, and some programs
						   seem to expect that padding to be there.

						   The following is designed to mimic the behavior
						   of these programs, padding to 96 bytes beyond the
						   end of the application proper. */

						/* get actual app length */
						app_length = 6 + (  (((uint32_t)(content->pages[0]->data[2])) << 24)
						                  | (((uint32_t)(content->pages[0]->data[3])) << 16)
						                  | (((uint32_t)(content->pages[0]->data[4])) <<  8)
						                  | ((uint32_t)(content->pages[0]->data[5])));

						/* remove any existing padding */
						while (page_length > 0 && content->pages[i]->data[page_length - 1] == 0xff)
						{
							page_length--;
						}

						extra_bytes = app_length + 96 - i * 0x4000 - page_length;

						/* don't add padding beyond the end of the page */
						if (page_length + extra_bytes >= 0x3fff)
						{
							extra_bytes = 0x3fff - page_length;
						}

						if (extra_bytes < 0)
						{
							extra_bytes = 0;
						}
						else if (extra_bytes > 96)
						{
							extra_bytes = 96;
						}
					}

					bytes_written += hex_block_write(f,
					  page_length, content->pages[i]->addr,
					  content->pages[i]->flag, content->pages[i]->data,
					  content->pages[i]->page, extra_bytes);
				}

				// final block
				bytes_written += hex_block_write(f, 0, 0, 0, NULL, 0, 0);
				if (fseek(f, -bytes_written - 4, SEEK_CUR)) goto tfwf;
				if (fwrite_long(f, bytes_written) < 0) goto tfwf;
				if (fseek(f, SEEK_END, 0L) ) goto tfwf;
			}
			else
			{
				// This looks like a TI-68k-style OS / FlashApp, containing flat data.
				if (fwrite(content->data_part, 1, content->data_length, f) < content->data_length) goto tfwf;
			}
		}
	}

	g_free(filename);
	fclose(f);
	return 0;

tfwf:	// release on exit
	tifiles_critical("%s: error writing file %s", __FUNCTION__, filename);
	g_free(filename);
	fclose(f);
	return ERR_FILE_IO;
}


/**************/
/* Displaying */
/**************/

/**
 * ti8x_content_display_backup:
 * @content: a Ti8xBackup structure.
 *
 * Display fields of a Ti8xBackup structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_content_display_backup(Ti8xBackup *content)
{
	if (content == NULL)
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
		return ERR_INVALID_FILE;
	}

	tifiles_info("BackupContent for TI-8x: %p", content);
	tifiles_info("Model:          %02X (%u)", content->model, content->model);
	tifiles_info("Signature:      %s", tifiles_calctype2signature(content->model));
	tifiles_info("Comment:        %s", content->comment);
	tifiles_info("Type:           %02X (%s)", content->type, tifiles_vartype2string(content->model, content->type));
	tifiles_info("Version:        %02X (%u)", content->version, content->version);
	tifiles_info("Mem address:    %04X (%u)", content->mem_address, content->mem_address);

	tifiles_info("data_length1:   %04X (%u)", content->data_length1, content->data_length1);
	tifiles_info("data_part1:     %p", content->data_part1);
	tifiles_info("data_length2:   %04X (%u)", content->data_length2, content->data_length2);
	tifiles_info("data_part2:     %p", content->data_part2);
	tifiles_info("data_length3:   %04X (%u)", content->data_length3, content->data_length3);
	tifiles_info("data_part3:     %p", content->data_part3);

	if (content->model == CALC_TI86)
	{
		tifiles_info("data_length4:   %04X (%u)", content->data_length4, content->data_length4);
		tifiles_info("data_part4:     %p", content->data_part4);
	}

	tifiles_info("Checksum:       %04X (%u)", content->checksum, content->checksum);

	return 0;
}

/**
 * ti8x_content_display_flash:
 * @content: a Ti8xFlash structure.
 *
 * Display fields of a Ti8xFlash structure.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_content_display_flash(Ti8xFlash *content)
{
	Ti8xFlash *ptr;

	for (ptr = content; ptr != NULL; ptr = ptr->next)
	{
		tifiles_info("FlashContent for TI-8x: %p", ptr);
		tifiles_info("Model:           %02X (%u)", ptr->model, ptr->model);
		tifiles_info("Signature:       %s", tifiles_calctype2signature(ptr->model));
		tifiles_info("model_dst:       %02X (%u)", ptr->model_dst, ptr->model_dst);
		tifiles_info("Revision:        %u.%u", ptr->revision_major, ptr->revision_minor);
		tifiles_info("Flags:           %02X", ptr->flags);
		tifiles_info("Object type:     %02X", ptr->object_type);
		tifiles_info("Date:            %02X/%02X/%02X%02X", ptr->revision_day, ptr->revision_month, ptr->revision_year & 0xff, (ptr->revision_year & 0xff00) >> 8);
		tifiles_info("Name:            %s", ptr->name);
		tifiles_info("Device type:     %s", ptr->device_type == DEVICE_TYPE_83P ? "ti83+" : "ti73");
		switch (ptr->data_type)
		{
			case 0x23:
				tifiles_info("Data type:       OS data");
				break;
			case 0x24:
				tifiles_info("Data type:       APP data");
				break;
			case 0x20:
			case 0x25:
				tifiles_info("Data type:       certificate");
				break;
			case 0x3E:
				tifiles_info("Data type:       license");
				break;
			default:
				tifiles_info("Data type:       Unknown (send mail to tilp-users@lists.sf.net)");
				break;
		}
		tifiles_info("Hardware ID:     %02X (%u)", ptr->hw_id, ptr->hw_id);
		tifiles_info("Length:          %08X (%u)", ptr->data_length, ptr->data_length);
		tifiles_info("Data:            %p", ptr->data_part);
		tifiles_info("Number of pages: %i", ptr->num_pages);
		tifiles_info("Pages:           %p", ptr->pages);
		tifiles_info("Next:            %p", ptr->next);
	}

	return 0;
}

/**
 * ti8x_file_display:
 * @filename: a TI file.
 *
 * Determine file class and display internal content.
 *
 * Return value: an error code, 0 otherwise.
 **/
int ti8x_file_display(const char *filename)
{
	Ti8xRegular *content1;
	Ti8xBackup *content2;
	Ti8xFlash *content3;
	int ret;

	if (tifiles_file_is_flash(filename)) 
	{
		content3 = tifiles_content_create_flash(CALC_TI83P);
		ret = ti8x_file_read_flash(filename, content3);
		if (!ret)
		{
			ti8x_content_display_flash(content3);
			tifiles_content_delete_flash(content3);
		}
	} 
	else if (tifiles_file_is_regular(filename)) 
	{
		content1 = tifiles_content_create_regular(CALC_NONE);
		ret = ti8x_file_read_regular(filename, content1);
		if (!ret)
		{
			tifiles_file_display_regular(content1);
			tifiles_content_delete_regular(content1);
		}
	} 
	else if (tifiles_file_is_backup(filename)) 
	{
		content2 = tifiles_content_create_backup(CALC_NONE);
		ret = ti8x_file_read_backup(filename, content2);
		if (!ret)
		{
			ti8x_content_display_backup(content2);
			tifiles_content_delete_backup(content2);
		}
	}
	else 
	{
		tifiles_info("Unknown file type !");
		return ERR_BAD_FILE;
	}

	return ret;
}

#endif
