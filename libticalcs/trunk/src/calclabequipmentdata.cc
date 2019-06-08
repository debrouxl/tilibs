/* Hey EMACS -*- linux-c -*- */

/*  libticalcs2 - hand-helds support library, a part of the TILP project
 *  Copyright (C) 2019-2026  Lionel Debroux
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
	Lab equipment list data conversion support: wire formats <-> text form <-> numeric values.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>
#include <cfloat>
#include <math.h>

// Range and precision checks:
// 1) the TI-85/86 raw format covers +/-999 decades. Verify at compile time that long double supports that range. x87 80-bit and binary128 do.
// On platforms where it doesn't, decoded values beyond the native range will saturate towards +/-inf or +/-0.
#if defined(LDBL_MAX_10_EXP) && defined(LDBL_MIN_10_EXP) && (LDBL_MAX_10_EXP < 999 || LDBL_MIN_10_EXP > -999)
#warning "long double cannot represent the full +/-999 decade range of the TI-85/86 format"
#endif

// 2) verify that the mantissa is large enough to hold all 14 BCD digits exactly.
#if defined(LDBL_MANT_DIG) && LDBL_MANT_DIG < 47
#warning "long double mantissa may not hold all 14 BCD digits exactly"
#endif

#include <types82.h>
#include <types85.h>

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "calclabequipmentdata.h"

/////////////----------------

// Exact decoded representation of one TI real number: sign, the 14 BCD mantissa digits d1..d14 as
// characters, and the decimal exponent of the leading digit.
typedef struct
{
	int negative;
	char digits[14];
	int32_t exp10;
} tixx_ti_real_value;

// Convert one pre-validated, already-lexed decimal number (optional sign, digits with at most one
// dot, optional exponent) into a TI real number:
// - 9-byte format (TI-73/82/83/83+/84+): sign+flags byte, 1-byte +/-99 exponent biased by 0x80, 7-byte BCD mantissa;
// - 10-byte format (TI-85/86): sign+flags byte, 2-byte little-endian +/-999 exponent biased by 0xFC00, 7-byte BCD mantissa.
// In both forms, bit 7 of the first byte is the sign, and the BCD digits represent d1.d2d3d4...d14 * 10^(exponent - bias).
// We need to operate directly on the decimal representation, because the 85/86 format covers a wider range than the C double type does.
// That's what the calculators themselves do anyway.
// Encode an exact representation (sign, 14 BCD digits d1..d14, decimal exponent of the leading
// digit) into a TI real number. The zero value is stored canonically: all-zero mantissa with the
// bias value in the exponent field, positive.
static void tixx_encode_ti_real_digits(int negative, const char digits[14], int32_t exp10, uint8_t * out, int is_8586)
{
	uint8_t * ptr = out;

	memset(out, 0, is_8586 ? 10 : 9);
	*ptr++ = negative ? 0x80 : 0x00; // We don't care about complex values here.
	if (is_8586)
	{
		const uint16_t field = (uint16_t)(0xFC00 + exp10);
		*ptr++ = field & 0xFF;
		*ptr++ = (field >> 8) & 0xFF;
	}
	else
	{
		*ptr++ = (uint8_t)(0x80 + exp10);
	}
	for (int i = 0; i < 7; i++)
	{
		*ptr++ = (uint8_t)((((digits[2 * i] - '0') << 4) | (digits[2 * i + 1] - '0')) & 0xFF);
	}
}

static int tixx_parse_decimal_to_ti_real(const char * src, uint8_t * out, int is_8586)
{
	int ret = 0;
	char digits[14];        // mantissa digits, as characters
	char significant[15];   // first significant digits of the input, enough for rounding
	long n_int = 0;         // number of digits before the dot, leading zeroes included
	long pos = 0;           // position of the current digit in the whole digit sequence
	long first_sig = -1;    // position of the first non-zero digit
	long exponent = 0;
	int significant_count = 0;
	int seen_dot = 0;
	int negative = 0;
	long exp10;
	const char * p = src;

	if (*src == 0 || *src == ' ')
	{
		// Avoid silently converting an empty token to zero.
		return ERR_INVALID_PARAMETER;
	}

	// Extract the significant digits and the scale of the number.
	if (*p == '-')
	{
		negative = true;
		p++;
	}
	else if (*p == '+')
	{
		p++;
	}
	for (; *p != 0 && *p != ' '; p++)
	{
		if (*p == '.')
		{
			seen_dot = 1;
		}
		else if (*p == 'e' || *p == 'E')
		{
			int exp_negative = 0;
			p++;
			if (*p == '-')
			{
				exp_negative = true;
				p++;
			}
			else if (*p == '+')
			{
				p++;
			}
			for (; *p >= '0' && *p <= '9'; p++)
			{
				if (exponent <= 100000) // clamp beyond supported range
				{
					exponent = exponent * 10 + (*p - '0');
				}
			}
			exponent = exp_negative ? -exponent : exponent;
			break;
		}
		else // digit
		{
			if (first_sig < 0 && *p != '0')
			{
				first_sig = pos;
			}
			if (first_sig >= 0 && significant_count < (int)sizeof(significant))
			{
				significant[significant_count++] = *p;
			}
			pos++;
			if (!seen_dot)
			{
				n_int++;
			}
		}
	}

	if (first_sig < 0)
	{
		// The value is zero: stored with an all-zero mantissa and the bias value in the exponent field.
		// A negative zero normalizes to positive zero.
		memset(digits, '0', sizeof(digits));
		tixx_encode_ti_real_digits(0, digits, 0, out, is_8586);
	}
	else
	{
		// With value written as significant_digits * 10^dec_exp, the normalization into
		// d1.d2...d14 * 10^exp10 makes exp10 independent of the number of significant digits.
		exp10 = n_int + exponent - first_sig - 1;

		// Fill the 14-digit mantissa with the available significant digits, right-padding with zeroes,
		// then round half-up using the 15th significant digit when one exists. Like the calculators'
		// own decimal parsers, the rounding decision uses that single lookahead digit: digits beyond
		// the 15th one do not influence the result.
		memset(digits, '0', sizeof(digits));
		memcpy(digits, significant, (size_t)(significant_count < (int)sizeof(digits) ? significant_count : sizeof(digits)));
		if (significant_count == (int)sizeof(significant) && significant[sizeof(significant) - 1] >= '5')
		{
			int carry = 1;
			for (int i = (int)sizeof(digits) - 1; i >= 0 && carry; i--)
			{
				if (digits[i] == '9')
				{
					digits[i] = '0';
				}
				else
				{
					digits[i]++;
					carry = 0;
				}
			}
			if (carry)
			{
				// All nine digits carried over: 999...9 + 1 = 10^14, i.e. 10^13 with the exponent bumped.
				digits[0] = '1';
				memset(digits + 1, '0', sizeof(digits) - 1);
				exp10++;
			}
		}

		if (exp10 > (is_8586 ? 999 : 99) || exp10 < (is_8586 ? -999 : -99))
		{
			ret = ERR_OUT_OF_RANGE;
		}
		else
		{
			tixx_encode_ti_real_digits(negative, digits, (int32_t)exp10, out, is_8586);
		}
	}

	return ret;
}

// Decode one TI real number into its exact digit/exponent representation:
// - 9-byte form (TI-73/82/83/83+/84+): sign+flags byte, 1-byte +/-99 exponent biased by 0x80, 7-byte BCD mantissa;
// - 10-byte form (TI-85/86): sign+flags byte, 2-byte little-endian +/-999 exponent biased by 0xFC00, 7-byte BCD mantissa.
// In both forms, bit 7 of the first byte is the sign and the BCD digits represent d1.d2d3d4...d14 * 10^(exponent - bias).
// A zero mantissa decodes as positive zero, whatever the other fields contain.
static int tixx_decode_ti_real_digits(const uint8_t * real, int is_8586, tixx_ti_real_value * out)
{
	int ret = 0;
	const uint8_t * ptr = real + (is_8586 ? 3 : 2);
	int i;

	memset(out, 0, sizeof(*out));
	for (i = 0; i < 7; i++)
	{
		out->digits[2 * i]     = (char)('0' + ((*ptr >> 4) & 0xF));
		out->digits[2 * i + 1] = (char)('0' + (*ptr++ & 0xF));
	}
	if (memcmp(out->digits, "00000000000000", sizeof(out->digits)))
	{
		out->exp10 = is_8586 ? (((int32_t)real[1] | ((int32_t)real[2] << 8)) - 0xFC00) : ((int32_t)real[1] - 0x80);
		if (out->exp10 > 999 || out->exp10 < -999)
		{
			ret = ERR_OUT_OF_RANGE;
		}
		else
		{
			out->negative = (real[0] & 0x80) ? !0 : 0;
		}
	}

	return ret;
}

// Convert the exact representation to a long double (see tixx_decode_ti_real()).
static long double tixx_ti_real_to_long_double(const tixx_ti_real_value * value)
{
	long double mantissa = 0;

	for (int i = 0; i < 14; i++)
	{
		mantissa = mantissa * 10 + (value->digits[i] - '0');
	}
	return (value->negative ? -1.0L : 1.0L) * mantissa * powl(10.0L, value->exp10 - 13);
}

// Convert the exact representation to a long double.
// On platforms whose long double covers the +/-999 decade range of the 10-byte format (x87 80-bit,
// IEEE754 binary128), every value is recovered essentially exactly; elsewhere values beyond the
// range of doubles saturate towards +/-inf or +/-0.
static int tixx_decode_ti_real(const uint8_t * real, int is_8586, long double * out)
{
	tixx_ti_real_value value;
	long double mantissa = 0;

	int ret = tixx_decode_ti_real_digits(real, is_8586, &value);
	if (!ret)
	{
		for (int i = 0; i < 14; i++)
		{
			mantissa = mantissa * 10 + (value.digits[i] - '0');
		}
		*out = (value.negative ? -1.0L : 1.0L) * mantissa * powl(10.0L, value.exp10 - 13);
	}
	return ret;
}

// Render the exact representation losslessly as a decimal string using '.' as decimal separator
// regardless of the current locale: plain notation [-]ddd[.fff] whenever the decimal point falls
// inside the 14 stored digits, scientific notation [-]d.fff e+/-nnn at the extremes of the +/-999
// decade range of the TI-85/86 format. Trailing fractional zeroes are trimmed. Returns the number
// of characters written.
static int tixx_format_ti_real_value(char * buf, const tixx_ti_real_value * value)
{
	size_t used = 0;
	int last = 13;
	int i;

	while (last > 0 && value->digits[last] == '0')
	{
		last--;
	}
	if (value->negative)
	{
		buf[used++] = '-';
	}

	if (value->exp10 >= 0 && value->exp10 <= 13)
	{
		// Plain notation: the decimal point falls between two stored digits.
		for (i = 0; i <= value->exp10; i++)
		{
			buf[used++] = value->digits[i];
		}
		if (last > value->exp10)
		{
			buf[used++] = '.';
			for (i = value->exp10 + 1; i <= last; i++)
			{
				buf[used++] = value->digits[i];
			}
		}
	}
	else if (value->exp10 < 0 && value->exp10 >= -(int)(sizeof(value->digits) - 1))
	{
		// Plain notation with leading zeroes after the decimal point.
		buf[used++] = '0';
		buf[used++] = '.';
		for (i = 0; i < -value->exp10 - 1; i++)
		{
			buf[used++] = '0';
		}
		for (i = 0; i <= last; i++)
		{
			buf[used++] = value->digits[i];
		}
	}
	else
	{
		// Scientific notation for the extremes of the range.
		buf[used++] = value->digits[0];
		if (last > 0)
		{
			buf[used++] = '.';
			for (i = 1; i <= last; i++)
			{
				buf[used++] = value->digits[i];
			}
		}
		// ticalcs_slprintf() can't be used here: its str argument wouldn't survive being an expression.
		const int printed = snprintf(buf + used, 16, "e%+d", value->exp10);
		if (printed > 0)
		{
			used += printed;
		}
	}
	buf[used] = 0;
	return (int)used;
}

// Parse a "{v1,v2,...}" lab equipment data list string in place: validate the reduced grammar
// (see below), count the items, turn '{' and ',' into separators and '}' into NUL. On success,
// @state ends as 7; on parse failure, it ends as 5.
// Simplified grammar:
// LIST: '{' BODY '}'
// BODY: NUMBER | (NUMBER ',')+ NUMBER
// NUMBER: [+-]? (DIGITS ['.' DIGITS*]? | '.' DIGITS) (('e'|'E') [+-]? DIGITS)?
//
// Operates on four local variables of the caller: uint8_t * ptr (cursor), uint8_t * orig_ptr
// (start of the string copy), unsigned int state (FSM state) and uint32_t items (item count).
#define TIXX_PARSE_LAB_EQUIPMENT_DATA_LIST_STRING() \
	do \
	{ \
		state = 0; \
		items = 0; \
		ptr = orig_ptr; \
		do \
		{ \
			uint8_t c = *ptr; \
			switch (state) \
			{ \
				/* We'd need more states if we wanted to cope with spaces before and after '{', ',' and '}'. */ \
				case 0: /* Expect the leading '{'. */ \
					if (c == '{') { *ptr = ' '; state = 1; } /* Read leading {, turn it into space and go on. */ \
					else state = 5; /* else error */ \
					break; \
				case 1: /* Expect the first character of a number. */ \
					items++; \
					if (c == '-' || c == '+') state = 2; /* Read a sign. */ \
					else if (c == '.') state = 3; /* Read a leading dot: ".5". */ \
					else if (c >= '0' && c <= '9') state = 4; /* Read a first digit. */ \
					else state = 5; /* else error */ \
					break; \
				case 2: /* After the sign: expect a digit or a leading dot. */ \
					if (c == '.') state = 3; \
					else if (c >= '0' && c <= '9') state = 4; \
					else state = 5; /* else error */ \
					break; \
				case 3: /* After a dot: expect a digit. */ \
					if (c >= '0' && c <= '9') state = 9; \
					else state = 5; /* else error */ \
					break; \
				case 4: /* Integer digits, no dot seen yet. */ \
					if (c >= '0' && c <= '9') state = 4; \
					else if (c == '.') state = 3; /* Read a dot. */ \
					else if (c == 'e' || c == 'E') state = 6; /* Read an exponent. */ \
					else if (c == ',') { *ptr = ' '; state = 1; } /* Read comma, turn it into space and go on. */ \
					else if (c == '}') { *ptr = 0; state = 7; } /* Read trailing }, success. */ \
					else state = 5; /* else error. */ \
					break; \
				case 9: /* Fractional digits, a dot was already seen. */ \
					if (c >= '0' && c <= '9') state = 9; \
					else if (c == 'e' || c == 'E') state = 6; \
					else if (c == ',') { *ptr = ' '; state = 1; } \
					else if (c == '}') { *ptr = 0; state = 7; } \
					else state = 5; /* else error. */ \
					break; \
				case 6: /* After 'e' or 'E': optional sign, then digits. */ \
					if (c == '-' || c == '+') state = 10; \
					else if (c >= '0' && c <= '9') state = 8; \
					else state = 5; /* else error */ \
					break; \
				case 10: /* After the exponent sign: expect a digit. */ \
					if (c >= '0' && c <= '9') state = 8; \
					else state = 5; /* else error */ \
					break; \
				case 8: /* Exponent digits. */ \
					if (c >= '0' && c <= '9') state = 8; \
					else if (c == ',') { *ptr = ' '; state = 1; } \
					else if (c == '}') { *ptr = 0; state = 7; } \
					else state = 5; /* else error. */ \
					break; \
				default: \
					break; \
			} \
			ptr++; \
		} while (state != 7 && state != 5); \
	} \
	while (0)

// Parse a "{v1,v2,...}" list string and produce raw list data in the wire format used by TI-Z80
// calculators ([number of elements as 16-bit LE], [9-byte / 10-byte real per element depending on the model]),
// so that the result is in the exact wire format and can be sent back as-is.
static int tixx_string_to_z80ish_raw_list_impl(const char * lab_equipment_data, int is_8586, CalcLabEquipmentData * out_data)
{
	int ret;
	uint8_t * orig_ptr, * ptr;
	unsigned int state;
	uint32_t items;
	uint8_t * converted_data = nullptr;
	const size_t real_size = is_8586 ? 10 : 9;
	const uint32_t max_items = (uint32_t)((0xFFFF - 2) / real_size);

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	orig_ptr = (uint8_t *)g_strdup(lab_equipment_data);
	if (nullptr == orig_ptr)
	{
		return ERR_MALLOC;
	}

	TIXX_PARSE_LAB_EQUIPMENT_DATA_LIST_STRING();

	if (state != 7)
	{
		ticalcs_warning("%s", _("Failed to parse lab equipment data list string"));
		g_free((void *)orig_ptr);
		return ERR_INVALID_PARAMETER;
	}

	if (items == 0)
	{
		// Empty lists are rejected on the sending path: they serve no useful purpose.
		ticalcs_warning("%s", _("Refusing to send an empty list"));
		g_free((void *)orig_ptr);
		return ERR_INVALID_PARAMETER;
	}

	if (items > max_items || 2 + items * real_size > 0xFFFF)
	{
		// Too many items, or an oversized list: the XDP packet length is 16-bit.
		ticalcs_warning("%s", _("Too many items in lab equipment data list string"));
		g_free((void *)orig_ptr);
		return ERR_INVALID_PARAMETER;
	}

	converted_data = (uint8_t *)g_malloc(2 + items * real_size);
	if (nullptr == converted_data)
	{
		g_free((void *)orig_ptr);
		return ERR_MALLOC;
	}
	converted_data[0] =  items        & 0xFF;
	converted_data[1] = (items >>  8) & 0xFF;

	// Second pass: convert each space-separated token into the target floating-point format.
	const char * src = (const char *)orig_ptr;
	while (*src == ' ')
	{
		src++;
	}
	for (uint32_t i = 0; i < items; i++)
	{
		ret = tixx_parse_decimal_to_ti_real(src, converted_data + 2 + i * real_size, is_8586);
		if (ret)
		{
			goto err;
		}
		while (*src != 0 && *src != ' ')
		{
			src++;
		}
		while (*src == ' ')
		{
			src++;
		}
	}
	if (*src != 0)
	{
		ret = ERR_INVALID_PARAMETER;
		goto err;
	}
	g_free((void *)orig_ptr);

	ticalcs_info("%s", _("Successfully parsed lab equipment data list string"));
	ticalcs_fill_lab_equipment_data(out_data, is_8586 ? CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST : CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, (uint16_t)(2 + items * real_size), (uint16_t)items, converted_data, nullptr, is_8586 ? TI85_LIST : TI82_LIST, 0, 0);
	return 0;

err:
	g_free((void *)orig_ptr);
	g_free(converted_data);
	ticalcs_warning("%s", _("Failed to convert lab equipment data list string"));
	return ret;
}

// Parse a reduced version of the original input form, i.e. the {...} inside Send(), without spaces or anything else (for now), and produce the
// [number of elements as 32-bit LE], [0x20 header], [characters making up the list with ',' replaced by ' '], [trailing 0x00]
// form which is transmitted on the wire by at least the 89, 92+, V200 and 89T (not checked the 92) for integer items.
// (negative numbers are transmitted verbatim, e.g. Send({3,-1}) yields " 3 -1")
static int tixx_string_to_ti68k_raw_list_impl(const char * lab_equipment_data, CalcLabEquipmentData * out_data)
{
	int ret;
	uint8_t * orig_ptr, * ptr;
	unsigned int state;
	uint32_t items;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	orig_ptr = (uint8_t *)g_strdup(lab_equipment_data);
	if (nullptr == orig_ptr)
	{
		return ERR_MALLOC;
	}
	//fprintf(stderr, "%s\n", ptr);

	TIXX_PARSE_LAB_EQUIPMENT_DATA_LIST_STRING();

	if (state == 7)
	{
		const size_t payload_size = (size_t)(ptr - orig_ptr);
		uint8_t * converted_data;
		if (items >= 32768 || 4 + payload_size > 65535)
		{
			// Too many items, or an oversized list.
			ticalcs_warning("%s", _("Failed to parse lab equipment data list string"));
			g_free((void *)orig_ptr);
			ret = ERR_INVALID_PARAMETER;
		}
		else
		{
			// Prefix the parsed payload with the number of elements as 32-bit little-endian, so that the
			// result is in the exact wire format and can be sent back as-is.
			converted_data = (uint8_t *)g_malloc(4 + payload_size);
			if (nullptr == converted_data)
			{
				g_free((void *)orig_ptr);
				ret = ERR_MALLOC;
			}
			else
			{
				converted_data[0] =  items        & 0xFF;
				converted_data[1] = (items >>  8) & 0xFF;
				converted_data[2] = (items >> 16) & 0xFF;
				converted_data[3] = (items >> 24) & 0xFF;
				memcpy(converted_data + 4, orig_ptr, payload_size);

				ticalcs_info("%s", _("Successfully parsed lab equipment data list string"));
				ticalcs_fill_lab_equipment_data(out_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST,
				                                (uint16_t)(4 + payload_size), (uint16_t)items, (const uint8_t *)converted_data, nullptr, 4, 0, 0);

				g_free((void *)orig_ptr);
				ret = 0;
			}
		}
	}
	else
	{
		ticalcs_warning("%s", _("Failed to parse lab equipment data list string"));
		g_free((void *)orig_ptr);
		ret = ERR_INVALID_PARAMETER;
	}

	return ret;
}

// Convert a raw list in the wire format used by TI-Z80 calculators ([number of elements as 16-bit
// LE], [9-byte real per element]) or by TI-85/86 calculators ([number of elements as 16-bit LE],
// [10-byte real per element]) into a "{v1,v2,...}" string, and into an array of doubles.
// Note that the Get direction of the TI-73/83/83+/84+ lab equipment protocol produces 10-byte
// elements as well, see tixx_get_lab_equipment_data_legacy() in calclabequipmentlegacy.cc.
static int tixx_z80ish_raw_list_to_string_impl(const CalcLabEquipmentData * lab_equipment_data, int is_8586, uint32_t * item_count, long double ** raw_values, char ** out_data)
{
	int ret;
	uint16_t items;
	const uint8_t * ptr;
	const size_t real_size = is_8586 ? 10 : 9;
	char * deststr = nullptr;
	size_t deststrsize;
	size_t offset;
	tixx_ti_real_value * vals = nullptr;
	long double * fpvals = nullptr;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	*item_count = 0;
	*raw_values = nullptr;
	*out_data = nullptr;

	if (lab_equipment_data->type != (is_8586 ? CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST : CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST))
	{
		return ERR_INVALID_PARAMETER;
	}
	// The raw list data carries its own 16-bit little-endian item count, right before the payload.
	if (lab_equipment_data->size < 2 || nullptr == lab_equipment_data->data)
	{
		return ERR_INVALID_PACKET;
	}
	items = (uint16_t)(lab_equipment_data->data[0] | (((uint16_t)lab_equipment_data->data[1]) << 8));
	if (items != lab_equipment_data->items || (uint32_t)lab_equipment_data->size != 2 + (uint32_t)items * real_size)
	{
		ticalcs_critical("%s", _("Inconsistent item count in lab equipment list data"));
		return ERR_INVALID_PACKET;
	}
	// A list of 32K items would be over 64 KB, which is invalid, due to max memory block / variable size.
	if (items >= 32768)
	{
		return ERR_INVALID_PARAMETER;
	}
	else if (items == 0)
	{
		deststr = g_strdup("{}");
		if (nullptr == deststr)
		{
			return ERR_MALLOC;
		}
		*item_count = items;
		// *raw_values already set above.
		*out_data = deststr;
		return 0;
	}

	vals = (tixx_ti_real_value *)g_malloc(items * sizeof(*vals));
	fpvals = (long double *)g_malloc(items * sizeof(*fpvals));
	if (nullptr == vals || nullptr == fpvals)
	{
		ret = ERR_MALLOC;
		goto err;
	}

	ptr = lab_equipment_data->data + 2;
	for (uint16_t i = 0; i < items; i++)
	{
		ret = tixx_decode_ti_real_digits(ptr + i * real_size, is_8586, &vals[i]);
		if (!ret)
		{
			fpvals[i] = tixx_ti_real_to_long_double(&vals[i]);
		}
		else
		{
			goto err;
		}
	}

	// Build the "{v1,v2,...,vN}" output string, computing the required buffer size upfront.
	deststrsize = 2; // '{', and the closing '}' replacing the trailing ',' of the last item.
	for (uint16_t j = 0; j < items; j++)
	{
		char tmp[32];
		const int printed = tixx_format_ti_real_value(tmp, &vals[j]);
		if (printed <= 0)
		{
			ret = ERR_INVALID_PACKET;
			goto err;
		}
		deststrsize += printed + 1; // value, plus its trailing ','
	}
	deststr = (char *)g_malloc(deststrsize);
	if (nullptr == deststr)
	{
		ret = ERR_MALLOC;
		goto err;
	}
	deststr[0] = '{';
	offset = 1;
	for (uint16_t j = 0; j < items; j++)
	{
		const int printed = tixx_format_ti_real_value(deststr + offset, &vals[j]);
		if (printed <= 0)
		{
			ret = ERR_INVALID_PACKET;
			goto err;
		}
		deststr[offset + printed] = ',';
		offset += printed + 1;
	}
	deststr[offset - 1] = '}';
	deststr[offset] = 0;

	ret = 0;
	*item_count = items;
	*raw_values = fpvals;
	*out_data = deststr;

	g_free(vals);

	return ret;

err:
	g_free(vals);
	g_free(fpvals);
	g_free(deststr);
	return ret;
}

static int tixx_ti68k_raw_list_to_string_impl(const CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, long double ** raw_values, char ** out_data)
{
	int ret;
	uint16_t items;
	const uint8_t * ptr;
	char * srcstr = nullptr;
	char * numstr = nullptr;
	size_t srcstrsize;
	size_t * tok_off = nullptr;
	size_t * tok_len = nullptr;
	char * deststr = nullptr;
	size_t offset;
	size_t deststrsize;
	uint16_t i;
	long double * fpvals = nullptr;
	struct lconv * lc;
	const char * decimal_point_str;
	size_t decimal_point_len;
	const char * q_num_base, * q_src, * q_num;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	*item_count = 0;
	*raw_values = nullptr;
	*out_data = nullptr;

	// The raw TI-68k list format carries a 32-bit item count right before the payload.
	if (lab_equipment_data->size < 4)
	{
		return ERR_INVALID_PARAMETER;
	}
	// Cross-check the 32-bit count carried by the raw data against the @items field.
	const uint32_t count =     (uint32_t)lab_equipment_data->data[0]
			       | (((uint32_t)lab_equipment_data->data[1]) << 8)
			       | (((uint32_t)lab_equipment_data->data[2]) << 16)
			       | (((uint32_t)lab_equipment_data->data[3]) << 24);
	if (count != lab_equipment_data->items)
	{
		return ERR_INVALID_PARAMETER;
	}

	items = lab_equipment_data->items;
	// A list of 32K items would be over 64 KB, which is invalid, due to max memory block / variable size.
	if (items >= 32768)
	{
		return ERR_INVALID_PARAMETER;
	}
	else if (items == 0)
	{
		deststr = g_strdup("{}");
		if (nullptr == deststr)
		{
			return ERR_MALLOC;
		}
		*item_count = items;
		// *raw_values already set above.
		*out_data = deststr;
		return 0;
	}

	// Copy the payload to a NUL-terminated buffer, so that parsing is bounded by the announced size.
	ptr = lab_equipment_data->data + 4;
	srcstrsize = lab_equipment_data->size - 4;
	srcstr = (char *)g_malloc(srcstrsize + 1);
	if (nullptr == srcstr)
	{
		return ERR_MALLOC;
	}
	memcpy(srcstr, ptr, srcstrsize);
	srcstr[srcstrsize] = 0;

	fpvals = (long double *)g_malloc(items * sizeof(*fpvals));
	tok_off = (size_t *)g_malloc(items * sizeof(*tok_off));
	tok_len = (size_t *)g_malloc(items * sizeof(*tok_len));
	if (nullptr == fpvals || nullptr == tok_off || nullptr == tok_len)
	{
		ret = ERR_MALLOC;
		goto err;
	}

	// strtold() parses numbers according to the current locale, whereas the wire format always uses
	// '.' as the decimal point: parse values from a copy of the payload with the locale decimal
	// point substituted, while @srcstr stays pristine so that the output string preserves the wire
	// text without any loss.
	lc = localeconv();
	decimal_point_str = lc->decimal_point;
	decimal_point_len = strlen(decimal_point_str);
	if (decimal_point_len != 1 || decimal_point_str[0] != '.')
	{
		size_t dots = 0;
		for (size_t i = 0; i < srcstrsize; i++)
		{
			if (srcstr[i] == '.')
			{
				dots++;
			}
		}
		if (dots > 0)
		{
			numstr = (char *)g_malloc(srcstrsize + dots * (decimal_point_len - 1) + 1);
			if (nullptr == numstr)
			{
				ret = ERR_MALLOC;
				goto err;
			}
			size_t w = 0;
			for (size_t i = 0; i < srcstrsize; i++)
			{
				if (srcstr[i] == '.')
				{
					memcpy(numstr + w, decimal_point_str, decimal_point_len);
					w += decimal_point_len;
				}
				else
				{
					numstr[w++] = srcstr[i];
				}
			}
			numstr[w] = 0;
		}
	}

	// Parse the announced number of space-separated items, capturing their wire text verbatim.
	q_num_base = numstr ? numstr : srcstr;
	q_src = srcstr;
	q_num = q_num_base;
	deststrsize = 0;
	i = 0;
	for (; i < items; i++)
	{
		char * endptr;
		while (*q_src == ' ') { q_src++; q_num++; }
		if (*q_src == 0)
		{
			break;
		}
		tok_off[i] = (size_t)(q_src - srcstr);
		tok_len[i] = strcspn(q_src, " ");
		deststrsize += tok_len[i] + 1; // value, plus its trailing ','
		fpvals[i] = strtold(q_num, &endptr);
		if (endptr == q_num)
		{
			// No number was parsed: the payload doesn't match the announced item count.
			ret = ERR_INVALID_PACKET;
			goto err;
		}
		q_src += tok_len[i];
		q_num += endptr - q_num;
	}
	if (i < items)
	{
		ret = ERR_INVALID_PACKET;
		goto err;
	}
	deststrsize += 2; // '{', and the closing '}' replacing the trailing ',' of the last item.

	deststr = (char *)g_malloc(deststrsize);
	if (nullptr == deststr)
	{
		ret = ERR_MALLOC;
		goto err;
	}
	deststr[0] = '{';
	offset = 1;
	for (uint16_t j = 0; j < items; j++)
	{
		memcpy(deststr + offset, srcstr + tok_off[j], tok_len[j]);
		offset += tok_len[j];
		deststr[offset++] = ',';
	}
	deststr[offset - 1] = '}';
	deststr[offset] = 0;

	g_free(tok_off);
	g_free(tok_len);
	g_free(srcstr);

	ret = 0;
	*item_count = items;
	*raw_values = fpvals;
	*out_data = deststr;

	return ret;

err:
	g_free(tok_off);
	g_free(tok_len);
	g_free(fpvals);
	g_free(deststr);
	g_free(srcstr);
	return ret;
}

// Convert a "{v1,v2,...}" list string into the raw list layout selected through
// @lab_equipment_data->type (#CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST,
// #CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST or #CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST).
// On success, @lab_equipment_data describes the produced raw list (and @vartype carries the
// protocol family's wire type byte).
int TICALL ticalcs_convert_lab_equipment_data_from_string(CalcLabEquipmentData * lab_equipment_data, const char * string_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(string_data);

	switch (lab_equipment_data->type)
	{
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST:
			return tixx_string_to_z80ish_raw_list_impl(string_data, 0, lab_equipment_data);
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST:
			return tixx_string_to_z80ish_raw_list_impl(string_data, !0, lab_equipment_data);
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST:
			return tixx_string_to_ti68k_raw_list_impl(string_data, lab_equipment_data);
		default:
			ticalcs_critical("%s", _("Unsupported raw list type"));
			return ERR_INVALID_PARAMETER;
	}
}

// Convert a raw list (TI-Z80 9-byte elements, TI-85/86 10-byte elements or TI-68k wire text) into
// a "{v1,v2,...}" string plus the decoded numeric values as long doubles. Dispatches on
// @lab_equipment_data->type. On success, @raw_values and @out_data are allocated and must be
// released by calling ticalcs_free_lab_equipment_data_related().
int TICALL ticalcs_convert_lab_equipment_data_to_string(const CalcLabEquipmentData * lab_equipment_data, uint32_t * item_count, long double ** raw_values, char ** out_data)
{
	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(item_count);
	VALIDATE_NONNULL(raw_values);
	VALIDATE_NONNULL(out_data);

	switch (lab_equipment_data->type)
	{
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST:
			return tixx_z80ish_raw_list_to_string_impl(lab_equipment_data, 0, item_count, raw_values, out_data);
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST:
			return tixx_z80ish_raw_list_to_string_impl(lab_equipment_data, !0, item_count, raw_values, out_data);
		case CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST:
			return tixx_ti68k_raw_list_to_string_impl(lab_equipment_data, item_count, raw_values, out_data);
		default:
			ticalcs_critical("%s", _("Unsupported raw list type"));
			return ERR_INVALID_PARAMETER;
	}
}

// Convert a raw list between the 9-byte element layout (TI-73/82/83/83+/84+) and the 10-byte one
// (TI-85/86). @out_data->type must be preset to the desired target type and differ from the source
// type; converting towards the 9-byte layout fails when an element's exponent falls outside its
// +/-99 range. On success, everything but @varname is filled in (@varname encodings differ between
// families, so selecting it is left to the caller); @data must be released by calling
// #ticalcs_calc_free_lab_equipment_data().
int TICALL ticalcs_convert_lab_equipment_data_raw_list_layout(CalcLabEquipmentData * lab_equipment_data, CalcLabEquipmentData * out_data)
{
	int ret;
	uint16_t items;
	const uint8_t * ptr;
	uint8_t * copy = nullptr;

	VALIDATE_NONNULL(lab_equipment_data);
	VALIDATE_NONNULL(out_data);

	const int src_is_8586 = (lab_equipment_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST);
	const int dst_is_8586 = (out_data->type == CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST);
	const size_t src_real_size = src_is_8586 ? 10 : 9;
	const size_t dst_real_size = dst_is_8586 ? 10 : 9;

	if (   (lab_equipment_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST && lab_equipment_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST)
	    || (out_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST && out_data->type != CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST)
	    || out_data->type == lab_equipment_data->type
	    || nullptr == lab_equipment_data->data
	    || lab_equipment_data->size < 2)
	{
		return ERR_INVALID_PARAMETER;
	}

	items = (uint16_t)(lab_equipment_data->data[0] | (((uint16_t)lab_equipment_data->data[1]) << 8));
	if (items != lab_equipment_data->items || (uint32_t)lab_equipment_data->size != 2 + (uint32_t)items * src_real_size)
	{
		ticalcs_critical("%s", _("Inconsistent item count in lab equipment list data"));
		return ERR_INVALID_PACKET;
	}

	copy = (uint8_t *)g_malloc(2 + items * dst_real_size);
	copy[0] = items & 0xFF;
	copy[1] = (items >> 8) & 0xFF;

	ptr = lab_equipment_data->data + 2;
	for (uint16_t i = 0; i < items; i++)
	{
		tixx_ti_real_value value;
		ret = tixx_decode_ti_real_digits(ptr + i * src_real_size, src_is_8586, &value);
		if (!ret && (value.exp10 > 999 || value.exp10 < -999 || (!dst_is_8586 && (value.exp10 > 99 || value.exp10 < -99))))
		{
			// The exponent doesn't fit into the target layout.
			ret = ERR_OUT_OF_RANGE;
		}
		if (!ret)
		{
			tixx_encode_ti_real_digits(value.negative, value.digits, value.exp10, copy + 2 + i * dst_real_size, dst_is_8586);
		}
		else
		{
			g_free(copy);
			return ret;
		}
	}

	ticalcs_fill_lab_equipment_data(out_data, out_data->type, (uint16_t)(2 + items * dst_real_size), items, copy, nullptr, dst_is_8586 ? 0x04 : 0x01, 0, 0);
	return 0;
}

/**
 * ticalcs_free_lab_equipment_data_related:
 * @data: pointer to the buffer to release.
 *
 * Releases a buffer allocated by a lab equipment conversion function.
 */
void TICALL ticalcs_free_lab_equipment_data_related(void * data)
{
	g_free(data);
}

void ticalcs_calc_free_lab_equipment_data(CalcLabEquipmentData * lab_equipment_data)
{
	if (nullptr != lab_equipment_data)
	{
		g_free((void *)(lab_equipment_data->data));
		g_free((void *)lab_equipment_data->varname);
		lab_equipment_data->data = nullptr;
		lab_equipment_data->varname = nullptr;
		lab_equipment_data->size = 0;
		lab_equipment_data->items = 0;
	}
}

