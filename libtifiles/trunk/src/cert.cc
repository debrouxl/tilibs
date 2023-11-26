/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
 *  Copyright (C) 2015  Benjamin Moody
 *  Copyright (C) 2015  Lionel Debroux
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

#include "tifiles.h"
#include "error.h"
#include "logging.h"

/**
 * tifiles_cert_field_get:
 * @data: input data supposedly containing a cert field.
 * @length: length of the input data.
 * @field_type: storage area for the field type, can be NULL.
 * @contents: storage area for the field contents, can be NULL.
 * @field_size: storage area for the field size, can be NULL.
 *
 * Parses certificate field into its components.
 *
 * Return value: whether retrieving / parsing the cert field succeeded.
 **/
int TICALL tifiles_cert_field_get(const uint8_t *data, uint32_t length, uint16_t *field_type, const uint8_t **contents, uint32_t *field_size)
{
	uint32_t field_len;
	uint32_t additional_len;

	// Initial sanity checks.
	if (data == nullptr)
	{
		tifiles_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (field_type == nullptr && contents == nullptr && field_size == nullptr)
	{
		tifiles_critical("%s: all output parameters are NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (length < 2)
	{
		tifiles_critical("%s: length is too small to contain a valid cert field", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}

	// Retrieve field ID and number of additional bytes we need to read for the field's size.
	const uint16_t field_id = (((uint16_t)data[0]) << 8) | data[1];
	switch (field_id & 0xF)
	{
		case 0xD: additional_len = 1; break;
		case 0xE: additional_len = 2; break;
		case 0xF: additional_len = 4; break;
		default:  additional_len = 0; break;
	}

	if (length < 2 + additional_len)
	{
		tifiles_critical("%s: length is too small for size bytes", __FUNCTION__);
		return ERR_CERT_TRUNCATED;
	}

	// Retrieve data size of field.
	switch (field_id & 0xF)
	{
		case 0xD:
		{
			field_len = (uint32_t)data[2];
		}
		break;

		case 0xE:
		{
			field_len = (((uint32_t)data[2]) << 8) | data[3];
		}
		break;

		case 0xF:
		{
			field_len = (((uint32_t)data[2]) << 24) | (((uint32_t)data[3]) << 16) | (((uint32_t)data[4]) << 8) | data[5];
		}
		break;

		default:
		{
			field_len = field_id & 0xF;
		}
		break;
	}

	if (length < 2 + additional_len + field_len)
	{
		tifiles_critical("%s: length is too small for data bytes", __FUNCTION__);
		return ERR_CERT_TRUNCATED;
	}

	if (field_type != nullptr)
	{
		// Don't mask out the size indication, it may be useful to the user.
		*field_type = field_id;
	}
	if (contents != nullptr)
	{
		*contents = data + 2 + additional_len;
	}
	if (field_size != nullptr)
	{
		*field_size = field_len;
	}

	return 0;
}

int TICALL tifiles_cert_field_next(const uint8_t **data, uint32_t *length)
{
	const uint8_t * contents;
	uint32_t field_size;

	// Initial sanity checks.
	if (data == nullptr)
	{
		tifiles_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (length == nullptr)
	{
		tifiles_critical("%s: length is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}

	const int ret = tifiles_cert_field_get(*data, *length, nullptr, &contents, &field_size);
	if (!ret)
	{
		*length -= contents + field_size - *data;
		*data = contents + field_size;
	}

	return ret;
}

int TICALL tifiles_cert_field_find(const uint8_t *data, uint32_t length, uint16_t field_type, const uint8_t **contents, uint32_t *field_size)
{
	int ret = 0;
	uint16_t ft;

	// Initial sanity checks.
	if (data == nullptr)
	{
		tifiles_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (length < 2)
	{
		tifiles_critical("%s: length is too small to contain a valid cert field", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}

	// Mask out the size indication, it is harmful for finding a field.
	field_type &= 0xFFF0;
	ft = 0xFFFF;

	while (!ret && ft != field_type)
	{
		ret = tifiles_cert_field_get(data, length, &ft, contents, field_size);
		ft &= 0xFFF0;
		if (!ret)
		{
			ret = tifiles_cert_field_next(&data, &length);
		}
	}

	return ret;
}

int TICALL tifiles_cert_field_find_path(const uint8_t *data, uint32_t length, const uint16_t *field_path, uint16_t field_path_len, const uint8_t **contents, uint32_t *field_size)
{
	int ret = 0;

	// Initial sanity checks.
	if (data == nullptr)
	{
		tifiles_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (field_path == nullptr)
	{
		tifiles_critical("%s: field_path is NULL", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (length < 2)
	{
		tifiles_critical("%s: length is too small to contain a valid cert field", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}
	if (field_path_len == 0)
	{
		tifiles_warning("%s: field path is empty", __FUNCTION__);
		return ERR_INVALID_PARAM;
	}

	while (field_path_len != 0 && !ret)
	{
		ret = tifiles_cert_field_find(data, length, *field_path, &data, &length);
		//tifiles_warning("%p\t%u", data, length);
		field_path++;
		field_path_len--;
		if (contents != nullptr)
		{
			*contents = data;
		}
		if (field_size != nullptr)
		{
			*field_size = length;
		}
	}

	return ret;
}
