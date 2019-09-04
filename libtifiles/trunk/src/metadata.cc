/* Hey EMACS -*- linux-c -*- */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 2026  Lionel Debroux
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
	This unit provides a way to retrieve the metadata of a TI file as an
	array of key/value pairs, under a parsable form.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <glib.h>
#include <ticonv.h>
#include "tifiles.h"
#include "files8x.h"
#include "files9x.h"
#include "error.h"
#include "logging.h"

/**
 * metadata_add:
 * @pairs: the key/value array being built (nullptr-terminated).
 * @count: the number of pairs currently stored in the array.
 * @newpairs: the number of key/value pairs to add.
 * @...: @newpairs pairs of (key, value) strings, i.e. 2*@newpairs variadic arguments.
 *
 * Append @newpairs key/value pairs to the array with a single reallocation.
 * The keys and the values are duplicated with strdup().
 *
 * Return value: 0 on success, ERR_MALLOC on memory allocation failure.
 **/
static int metadata_add(TifilesMetadataPair ** pairs, unsigned int * count, unsigned int newpairs, ...)
{
	va_list ap;
	TifilesMetadataPair * grown;
	unsigned int i;

	grown = (TifilesMetadataPair *)realloc(*pairs, (*count + newpairs + 1) * sizeof(TifilesMetadataPair));
	if (!grown)
	{
		return ERR_MALLOC;
	}
	*pairs = grown;

	va_start(ap, newpairs);
	for (i = 0; i < newpairs; i++)
	{
		const char * key = va_arg(ap, const char *);
		const char * value = va_arg(ap, const char *);
		char * k = strdup(key);
		char * v = strdup(value);

		if (!k || !v)
		{
			free(k);
			free(v);
			// Terminate the array properly, so that the pairs already stored
			// (and the array itself) can be freed by the caller.
			(*pairs)[*count].key = nullptr;
			(*pairs)[*count].value = nullptr;
			va_end(ap);
			return ERR_MALLOC;
		}
		(*pairs)[*count].key = k;
		(*pairs)[*count].value = v;
		(*count)++;
	}
	va_end(ap);
	(*pairs)[*count].key = nullptr;
	(*pairs)[*count].value = nullptr;
	return 0;
}

/**
 * metadata_read_magic:
 * @filename: the file name.
 * @magic: a 9-byte buffer receiving the file magic, NUL-terminated.
 *
 * Valid magic numbers are made of printable ASCII characters, hence the raw
 * bytes are usable as a string. Read 4 bytes first; if they match the ZIP
 * magic (as for tigroup files), stop there, otherwise read 4 more bytes.
 *
 * Return value: 0 on success, -1 otherwise.
 **/
static int metadata_read_magic(const char * filename, char * magic)
{
	FILE * f = fopen(filename, "rb");

	if (!f)
	{
		return -1;
	}
	if (fread(magic, 1, 4, f) != 4)
	{
		fclose(f);
		return -1;
	}
	// ZIP archives (TiGroup files) start with the "PK\x03\x04" magic.
	if (memcmp(magic, "PK\x03\x04", 4) != 0)
	{
		if (fread(magic + 4, 1, 4, f) != 4)
		{
			fclose(f);
			return -1;
		}
		magic[8] = 0;
	}
	else
	{
		magic[4] = 0;
	}
	fclose(f);
	return 0;
}

/**
 * metadata_read_byte_at:
 * @filename: the file name.
 * @offset: the byte offset to read.
 * @value: the byte read.
 *
 * Return value: 0 on success, -1 otherwise.
 **/
static int metadata_read_byte_at(const char * filename, long offset, uint8_t * value)
{
	FILE * f = fopen(filename, "rb");

	if (!f)
	{
		return -1;
	}
	if (fseek(f, offset, SEEK_SET) < 0 || fread(value, 1, 1, f) != 1)
	{
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}

/**
 * metadata_warning_add:
 * @warnings: a buffer receiving the space-separated warning tokens.
 * @size: the size of the @warnings buffer.
 * @warning: the token to append.
 **/
static void metadata_warning_add(char * warnings, size_t size, const char * warning)
{
	snprintf(warnings + strlen(warnings), size - strlen(warnings), "%s%s", warnings[0] ? " " : "", warning);
}

/**
 * metadata_fill_regular:
 * @pairs: the key/value array being built.
 * @count: the number of pairs currently stored in the array.
 * @content: the parsed content of a single/group file.
 * @magic: the file magic.
 *
 * Fill the array with the metadata of a single/group file, labelling the
 * class "single" or "group" according to the number of entries.
 *
 * Return value: 0 on success, ERR_MALLOC on memory allocation failure.
 **/
static int metadata_fill_regular(TifilesMetadataPair ** pairs, unsigned int * count, FileContent * content, const char * magic)
{
	char key[9][64];
	char value[6][64];
	unsigned int i;
	const char * fileclass = content->num_entries == 1 ? "single" : "group";

	snprintf(value[0], sizeof(value[0]), "0x%04X", content->checksum);
	snprintf(value[1], sizeof(value[1]), "0x%04X", content->stored_checksum);
	snprintf(value[2], sizeof(value[2]), "%u", content->num_entries);
	if (metadata_add(pairs, count, 7,
	                 "file.class", fileclass,
	                 "file.model", tifiles_model_to_string(content->model),
	                 "file.magic", magic,
	                 "file.comment", content->comment,
	                 "file.checksum.computed", value[0],
	                 "file.checksum.stored", value[1],
	                 "file.entries", value[2]) != 0)
	{
		return ERR_MALLOC;
	}

	for (i = 0; i < content->num_entries; i++)
	{
		VarEntry * ve = content->entries[i];
		const char * typestr = tifiles_vartype2string(content->model, ve->type);
		const char * ext = tifiles_vartype2fext(content->model, ve->type);

		snprintf(key[0], sizeof(key[0]), "entry[%u].name", i + 1);
		snprintf(key[1], sizeof(key[1]), "entry[%u].folder", i + 1);
		snprintf(key[2], sizeof(key[2]), "entry[%u].type", i + 1);
		snprintf(key[3], sizeof(key[3]), "entry[%u].type.name", i + 1);
		snprintf(key[4], sizeof(key[4]), "entry[%u].type.ext", i + 1);
		snprintf(key[5], sizeof(key[5]), "entry[%u].size", i + 1);
		snprintf(key[6], sizeof(key[6]), "entry[%u].version", i + 1);
		snprintf(key[7], sizeof(key[7]), "entry[%u].attr.archived", i + 1);
		snprintf(key[8], sizeof(key[8]), "entry[%u].attr.locked", i + 1);

		snprintf(value[0], sizeof(value[0]), "%u", ve->type);
		snprintf(value[1], sizeof(value[1]), "%u", ve->size);
		snprintf(value[2], sizeof(value[2]), "%u", ve->version);
		snprintf(value[3], sizeof(value[3]), "%d", ve->attr == ATTRB_ARCHIVED ? 1 : 0);
		snprintf(value[4], sizeof(value[4]), "%d", ve->attr == ATTRB_LOCKED ? 1 : 0);

		if (metadata_add(pairs, count, 9,
		                 key[0], ve->name,
		                 key[1], ve->folder,
		                 key[2], value[0],
		                 key[3], typestr ? typestr : "",
		                 key[4], ext ? ext : "",
		                 key[5], value[1],
		                 key[6], value[2],
		                 key[7], value[3],
		                 key[8], value[4]) != 0)
		{
			return ERR_MALLOC;
		}
	}
	return 0;
}

/**
 * metadata_fill_backup:
 * @pairs: the key/value array being built.
 * @count: the number of pairs currently stored in the array.
 * @content: the parsed content of a backup file.
 * @magic: the file magic.
 *
 * Fill the array with the metadata of a backup file.
 *
 * Return value: 0 on success, ERR_MALLOC on memory allocation failure.
 **/
static int metadata_fill_backup(TifilesMetadataPair ** pairs, unsigned int * count, BackupContent * content, const char * magic)
{
	char value[2][64];

	snprintf(value[0], sizeof(value[0]), "0x%04X", content->checksum);
	snprintf(value[1], sizeof(value[1]), "0x%04X", content->stored_checksum);
	return metadata_add(pairs, count, 7,
	                    "file.class", "backup",
	                    "file.model", tifiles_model_to_string(content->model),
	                    "file.magic", magic,
	                    "file.comment", content->comment,
	                    "file.checksum.computed", value[0],
	                    "file.checksum.stored", value[1],
	                    "file.entries", "1");
}

/**
 * metadata_fill_flash:
 * @pairs: the key/value array being built.
 * @count: the number of pairs currently stored in the array.
 * @content: the parsed content of a flash file.
 * @magic: the file magic.
 *
 * Fill the array with the metadata of a flash file.
 *
 * Return value: 0 on success, ERR_MALLOC on memory allocation failure.
 **/
static int metadata_fill_flash(TifilesMetadataPair ** pairs, unsigned int * count, FlashContent * content, const char * magic)
{
	return metadata_add(pairs, count, 5,
	                    "file.class", "flash",
	                    "file.model", tifiles_model_to_string(content->model),
	                    "file.magic", magic,
	                    "file.name", content->name,
	                    "file.entries", "1");
}

/**
 * metadata_fill_tigroup:
 * @pairs: the key/value array being built.
 * @count: the number of pairs currently stored in the array.
 * @content: the parsed content of a tigroup file.
 * @magic: the file magic.
 *
 * Fill the array with the metadata of a tigroup file.
 *
 * Return value: 0 on success, ERR_MALLOC on memory allocation failure.
 **/
static int metadata_fill_tigroup(TifilesMetadataPair ** pairs, unsigned int * count, TigContent * content, const char * magic)
{
	char key[10][64];
	char value[5][64];
	unsigned int i;

	snprintf(value[0], sizeof(value[0]), "%u", content->n_vars + content->n_apps);
	if (metadata_add(pairs, count, 5,
	                 "file.class", "tigroup",
	                 "file.model", tifiles_model_to_string(content->model),
	                 "file.magic", magic,
	                 "file.comment", content->comment ? content->comment : "",
	                 "file.entries", value[0]) != 0)
	{
		return ERR_MALLOC;
	}

	for (i = 0; i < content->n_vars; i++)
	{
		TigEntry * te = content->var_entries[i];
		// Degenerate members (e.g. a regular file with an empty data section)
		// parse successfully with no variable entry: emit what we know.
		VarEntry * ve = (te->content.regular && te->content.regular->num_entries) ? te->content.regular->entries[0] : nullptr;

		snprintf(key[0], sizeof(key[0]), "entry[%u].filename", i + 1);
		snprintf(key[1], sizeof(key[1]), "entry[%u].class", i + 1);

		if (!ve)
		{
			if (metadata_add(pairs, count, 2,
			                 key[0], te->filename,
			                 key[1], "regular") != 0)
			{
				return ERR_MALLOC;
			}
			continue;
		}
		const char * typestr = tifiles_vartype2string(content->model, ve->type);
		const char * ext = tifiles_vartype2fext(content->model, ve->type);

		snprintf(key[2], sizeof(key[2]), "entry[%u].name", i + 1);
		snprintf(key[3], sizeof(key[3]), "entry[%u].type", i + 1);
		snprintf(key[4], sizeof(key[4]), "entry[%u].type.name", i + 1);
		snprintf(key[5], sizeof(key[5]), "entry[%u].type.ext", i + 1);
		snprintf(key[6], sizeof(key[6]), "entry[%u].size", i + 1);
		snprintf(key[7], sizeof(key[7]), "entry[%u].version", i + 1);
		snprintf(key[8], sizeof(key[8]), "entry[%u].attr.archived", i + 1);
		snprintf(key[9], sizeof(key[9]), "entry[%u].attr.locked", i + 1);

		snprintf(value[0], sizeof(value[0]), "%u", ve->type);
		snprintf(value[1], sizeof(value[1]), "%u", ve->size);
		snprintf(value[2], sizeof(value[2]), "%u", ve->version);
		snprintf(value[3], sizeof(value[3]), "%d", ve->attr == ATTRB_ARCHIVED ? 1 : 0);
		snprintf(value[4], sizeof(value[4]), "%d", ve->attr == ATTRB_LOCKED ? 1 : 0);

		if (metadata_add(pairs, count, 10,
		                 key[0], te->filename,
		                 key[1], "regular",
		                 key[2], ve->name,
		                 key[3], value[0],
		                 key[4], typestr ? typestr : "",
		                 key[5], ext ? ext : "",
		                 key[6], value[1],
		                 key[7], value[2],
		                 key[8], value[3],
		                 key[9], value[4]) != 0)
		{
			return ERR_MALLOC;
		}
	}
	for (i = 0; i < content->n_apps; i++)
	{
		TigEntry * te = content->app_entries[i];
		FlashContent * flash = te->content.flash;

		snprintf(key[0], sizeof(key[0]), "entry[%u].filename", content->n_vars + i + 1);
		snprintf(key[1], sizeof(key[1]), "entry[%u].class", content->n_vars + i + 1);
		snprintf(key[2], sizeof(key[2]), "entry[%u].name", content->n_vars + i + 1);
		snprintf(key[3], sizeof(key[3]), "entry[%u].size", content->n_vars + i + 1);

		snprintf(value[0], sizeof(value[0]), "%u", flash->data_length);

		if (metadata_add(pairs, count, 4,
		                 key[0], te->filename,
		                 key[1], "flash",
		                 key[2], flash->name,
		                 key[3], value[0]) != 0)
		{
			return ERR_MALLOC;
		}
	}
	return 0;
}

/**
 * tifiles_file_get_metadata:
 * @filename: a TI file name.
 *
 * Parse the file and return its metadata as a nullptr-terminated array of
 * key/value pairs, whose keys are documented in the tifileutil(1) manual page.
 * The file is classified by its content (magic and type byte), the extension
 * being only cross-checked for the "file.warnings" key. The array and all its
 * strings are allocated with malloc()/strdup() and must be freed with
 * tifiles_metadata_free().
 *
 * Return value: the array of pairs, or nullptr on error (unknown file class,
 * unreadable file, or memory allocation failure).
 **/
TifilesMetadataPair * TICALL tifiles_file_get_metadata(const char *filename)
{
	TifilesMetadataPair * pairs = nullptr;
	unsigned int count = 0;
	char magic[9];
	char warnings[128] = "";
	FileClass ext_fileclass;
	FileClass content_fileclass = TIFILE_NONE;
	CalcModel ext_model;
	CalcModel content_model = CALC_NONE;
	CalcModel model;
	int ret;
	uint8_t type_byte = 0;

	if (!filename)
	{
		return nullptr;
	}
	if (metadata_read_magic(filename, magic) < 0)
	{
		return nullptr;
	}

	// Classify the file by its content: the magic, and the type byte.
	if (!memcmp(magic, "PK\x03\x04", 4))
	{
		content_fileclass = TIFILE_TIGROUP;
	}
	else if (!memcmp(magic, "**TIFL**", 8))
	{
		content_fileclass = TIFILE_FLASH;
	}
	else {
		content_model = tifiles_signature2calctype(magic);
		if (content_model == CALC_NONE)
		{
			return nullptr;
		}
		if (tifiles_calc_is_ti8x(content_model))
		{
			// TI-82 backups use type 0x0F, TI-73/83/83+ 0x13, TI-85/86 0x1D.
			metadata_read_byte_at(filename, 0x3B, &type_byte);
			if (type_byte == 0x0F || type_byte == 0x13 || type_byte == 0x1D)
			{
				content_fileclass = TIFILE_BACKUP;
			}
			else
			{
				content_fileclass = TIFILE_SINGLE;
			}
		}
		else
		{
			// TI-68k backups use type 0x1D @0x48, as verified against the
			// real Fargo installation backups.
			metadata_read_byte_at(filename, 0x48, &type_byte);
			if (type_byte == 0x1D)
			{
				content_fileclass = TIFILE_BACKUP;
			}
			else
			{
				content_fileclass = TIFILE_SINGLE;
			}
		}
	}

	// The extension-derived model and class, for cross-checking.
	ext_fileclass = tifiles_file_get_class(filename);
	ext_model = tifiles_file_get_model(filename);

	// The model used for parsing: the content-derived one, except for flash
	// (whose model is extension-derived) and tigroup files (ZIP archives).
	model = content_model;
	if (content_fileclass == TIFILE_FLASH)
	{
		model = ext_model;
	}
	if (model == CALC_NONE && content_fileclass != TIFILE_TIGROUP && content_fileclass != TIFILE_FLASH)
	{
		return nullptr;
	}

	// The version of the interchange format emitted by this function, consumed
	// by metadata -m apply.
	if (metadata_add(&pairs, &count, 1, "metadata.version", "1") != 0)
	{
		goto fail;
	}

	switch (content_fileclass)
	{
	case TIFILE_SINGLE:
	case TIFILE_GROUP:
	{
		FileContent * content = tifiles_content_create_regular(model);

		if (!content)
		{
			goto fail;
		}
		ret = tifiles_calc_is_ti8x(model) ? ti8x_file_read_regular(filename, content) : ti9x_file_read_regular(filename, content);
		if (ret)
		{
			tifiles_content_delete_regular(content);
			goto fail;
		}
		if (content->checksum != content->stored_checksum)
		{
			metadata_warning_add(warnings, sizeof(warnings), "checksum");
		}
		content_fileclass = content->num_entries == 1 ? TIFILE_SINGLE : TIFILE_GROUP;
		ret = metadata_fill_regular(&pairs, &count, content, magic);
		tifiles_content_delete_regular(content);
		if (ret)
		{
			goto fail;
		}
		break;
	}
	case TIFILE_BACKUP:
	{
		BackupContent * content = tifiles_content_create_backup(model);

		if (!content)
		{
			goto fail;
		}
		ret = tifiles_calc_is_ti8x(model) ? ti8x_file_read_backup(filename, content) : ti9x_file_read_backup(filename, content);
		if (ret)
		{
			tifiles_content_delete_backup(content);
			goto fail;
		}
		if (content->checksum != content->stored_checksum)
		{
			metadata_warning_add(warnings, sizeof(warnings), "checksum");
		}
		ret = metadata_fill_backup(&pairs, &count, content, magic);
		tifiles_content_delete_backup(content);
		if (ret)
		{
			goto fail;
		}
		break;
	}
	case TIFILE_FLASH:
	{
		FlashContent * content = tifiles_content_create_flash(model);
		uint8_t dev_type = 0, data_type = 0;

		if (!content)
		{
			goto fail;
		}
		if (model == CALC_NONE)
		{
			// Unknown model (e.g. unrecognized extension): resolve the family
			// from the flash device type.
			tifiles_file_has_tifl_header(filename, &dev_type, &data_type);
			ret = (dev_type == DEVICE_TYPE_89 || dev_type == DEVICE_TYPE_92P) ? ti9x_file_read_flash(filename, content) : ti8x_file_read_flash(filename, content);
		}
		else if (tifiles_calc_is_ti8x(model))
		{
			ret = ti8x_file_read_flash(filename, content);
		}
		else
		{
			ret = ti9x_file_read_flash(filename, content);
		}
		if (ret)
		{
			tifiles_content_delete_flash(content);
			goto fail;
		}
		ret = metadata_fill_flash(&pairs, &count, content, magic);
		tifiles_content_delete_flash(content);
		if (ret)
		{
			goto fail;
		}
		break;
	}
	case TIFILE_TIGROUP:
	{
		TigContent * content = tifiles_content_create_tigroup(model, 0);

		if (!content)
		{
			goto fail;
		}
		ret = tifiles_file_read_tigroup(filename, content);
		if (ret)
		{
			tifiles_content_delete_tigroup(content);
			goto fail;
		}
		ret = metadata_fill_tigroup(&pairs, &count, content, magic);
		tifiles_content_delete_tigroup(content);
		if (ret)
		{
			goto fail;
		}
		break;
	}
	default:
		goto fail;
	}

	// Mismatches between the extension and the content.
	if (   (ext_model == CALC_NONE && ext_fileclass != TIFILE_TIGROUP)
	    || (ext_model != CALC_NONE && content_model != CALC_NONE && ext_model != content_model)
	    || (ext_fileclass != TIFILE_NONE && content_fileclass != TIFILE_NONE && ext_fileclass != content_fileclass)) {
		metadata_warning_add(warnings, sizeof(warnings), "extension_mismatch");
	}
	if (warnings[0]) {
		if (metadata_add(&pairs, &count, 1, "file.warnings", warnings) != 0) {
			goto fail;
		}
	}

	return pairs;

fail:
	tifiles_metadata_free(pairs);
	return nullptr;
}

/**
 * tifiles_metadata_free:
 * @pairs: an array of key/value pairs returned by tifiles_file_get_metadata().
 *
 * Free the array and all its strings.
 **/
void TICALL tifiles_metadata_free(TifilesMetadataPair *pairs)
{
	unsigned int i;

	if (pairs)
	{
		for (i = 0; pairs[i].key != nullptr; i++)
		{
			free(pairs[i].key);
			free(pairs[i].value);
		}
		free(pairs);
	}
}

/**
 * tifiles_metadata_add_pair:
 * @pairs: the key/value array being built (nullptr-terminated, or nullptr).
 * @count: the number of pairs currently stored in the array.
 * @key: the key of the pair to add.
 * @value: the value of the pair to add.
 *
 * Append a single key/value pair to the array, duplicating the strings with
 * strdup(). The key must not already be present in the array.
 *
 * Return value: 0 on success, ERR_INVALID_PARAM if a parameter is nullptr,
 * ERR_MALLOC on memory allocation failure, ERR_DUPLICATE_KEY if the key is
 * already present.
 **/
int TICALL tifiles_metadata_add_pair(TifilesMetadataPair **pairs, unsigned int *count, const char *key, const char *value)
{
	TifilesMetadataPair *grown;
	char *k, *v;

	if (pairs == nullptr || count == nullptr || key == nullptr || value == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (tifiles_metadata_find_pair(*pairs, key) != nullptr)
	{
		return ERR_DUPLICATE_KEY;
	}
	grown = (TifilesMetadataPair *)realloc(*pairs, (*count + 2) * sizeof(TifilesMetadataPair));
	if (grown == nullptr)
	{
		return ERR_MALLOC;
	}
	*pairs = grown;
	k = strdup(key);
	v = strdup(value);
	if (k == nullptr || v == nullptr)
	{
		free(k);
		free(v);
		// Terminate the array properly, so that the pairs already stored
		// (and the array itself) can be freed by the caller.
		(*pairs)[*count].key = nullptr;
		(*pairs)[*count].value = nullptr;
		return ERR_MALLOC;
	}
	(*pairs)[*count].key = k;
	(*pairs)[*count].value = v;
	(*count)++;
	(*pairs)[*count].key = nullptr;
	(*pairs)[*count].value = nullptr;
	return 0;
}

/**
 * tifiles_metadata_find_pair:
 * @pairs: a key/value array (nullptr-terminated, or nullptr).
 * @key: the key to search for.
 *
 * Return value: the pair with the given key, or nullptr if it is not present
 * (or if a parameter is nullptr).
 **/
TifilesMetadataPair * TICALL tifiles_metadata_find_pair(TifilesMetadataPair *pairs, const char *key)
{
	unsigned int i;

	if (key == nullptr)
	{
		tifiles_critical("%s: an argument is NULL", __FUNCTION__);
		return nullptr;
	}
	for (i = 0; pairs != nullptr && pairs[i].key != nullptr; i++)
	{
		if (!strcmp(pairs[i].key, key))
		{
			return &pairs[i];
		}
	}
	return nullptr;
}
