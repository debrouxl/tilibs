/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
 *  Copyright (C) 2015 Lionel Debroux
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

#include "ticalcs.h"
#include "gettext.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

/**
 * ticalcs_screen_convert_bw_to_rgb888:
 * @src: source bitmap (1-bit pixels) to be converted.
 * @width: width of the bitmap in pixels, assumed to be be a multiple of 8.
 * @height: height of the bitmap in pixels
 * @dst: storage space for the converted bitmap, must be at least 3 * width * height bytes large
 *
 * Convert 1-bit B/W bitmap to RGB888, the output bitmap uses black and white pixels.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_screen_convert_bw_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	unsigned int i;

	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (i = 0; i < height; i++)
	{
		unsigned int j;
		for (j = 0; j < (width >> 3); j++)
		{
			uint8_t data = *src++;
			uint8_t mask = 0x80;
			unsigned int bit;
			for (bit = 0; bit < 8; bit++)
			{
				if (data & mask)
				{
					*dst++ = 0x00;
					*dst++ = 0x00;
					*dst++ = 0x00;
				}
				else
				{
					*dst++ = 0xFF;
					*dst++ = 0xFF;
					*dst++ = 0xFF;
				}
				mask >>= 1;
			}
		}
	}

	return 0;
}

/**
 * ticalcs_screen_convert_bw_to_blurry_rgb888:
 * @src: source bitmap (1-bit pixels) to be converted.
 * @width: width of the bitmap in pixels, assumed to be be a multiple of 8.
 * @height: height of the bitmap in pixels
 * @dst: storage space for the converted bitmap, must be at least 3 * width * height bytes large
 *
 * Convert 1-bit B/W bitmap to RGB888, the output bitmap uses a pair of colors mimicking old TI-Z80 / TI-68k screens.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_screen_convert_bw_to_blurry_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	unsigned int i;

	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (i = 0; i < height; i++)
	{
		unsigned int j;
		for (j = 0; j < (width >> 3); j++)
		{
			uint8_t data = *src++;
			uint8_t mask = 0x80;
			unsigned int bit;
			for (bit = 0; bit < 8; bit++)
			{
				if (data & mask)
				{
					*dst++ = 0x00;
					*dst++ = 0x00;
					*dst++ = 0x34;
				}
				else
				{
					*dst++ = 0xA8;
					*dst++ = 0xB4;
					*dst++ = 0xA8;
				}
				mask >>= 1;
			}
		}
	}

	return 0;
}

/**
 * ticalcs_screen_convert_gs4_to_rgb888:
 * @src: source bitmap (4-bit pixels) to be converted.
 * @width: width of the bitmap in pixels, assumed to be even.
 * @height: height of the bitmap in pixels
 * @dst: storage space for the converted bitmap, must be at least 3 * width * height bytes large
 *
 * Convert 4-bit grayscale bitmap to RGB888.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_screen_convert_gs4_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	unsigned int i;

	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (i = 0; i < height; i++)
	{
		unsigned int j;
		for (j = 0; j < width / 2; j++)
		{
			uint8_t data = *src++;
			uint8_t hi = data >> 4;
			uint8_t lo = data & 0x0f;

			*dst++ = hi << 4;
			*dst++ = hi << 4;
			*dst++ = hi << 4;

			*dst++ = lo << 4;
			*dst++ = lo << 4;
			*dst++ = lo << 4;
		}
	}

	return 0;
}

/**
 * ticalcs_screen_convert_rgb565le_to_rgb888:
 * @src: source bitmap (16-bit pixels) to be converted.
 * @width: width of the bitmap in pixels
 * @height: height of the bitmap in pixels
 * @dst: storage space for the converted bitmap, must be at least 3 * width * height bytes large
 *
 * Convert 16-bit little-endian RGB565 bitmap to RGB888.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_screen_convert_rgb565le_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	unsigned int i;

	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (i = 0; i < height; i++)
	{
		unsigned int j;
		for (j = 0; j < width; j++)
		{
			uint16_t data = (((uint16_t)(src[1])) << 8) | ((uint16_t)(src[0]));

			src += 2;
			*dst++ = ((data & 0xF800) >> 11) << 3;
			*dst++ = ((data & 0x07E0) >>  5) << 2;
			*dst++ = ((data & 0x001F) >>  0) << 3;
		}
	}

	return 0;
}

TIEXPORT3 int TICALL ticalcs_screen_convert_native_to_rgb888(CalcPixelFormat format, const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	int ret;

	VALIDATE_NONNULL(src);
	// width and height are validated by other ticalcs_screen_convert_* functions.
	VALIDATE_NONNULL(dst);

	switch (format)
	{
		case CALC_PIXFMT_MONO:
		{
			ret = ticalcs_screen_convert_bw_to_rgb888(src, width, height, dst);
		}
		break;

		case CALC_PIXFMT_GRAY_4:
		{
			ret = ticalcs_screen_convert_gs4_to_rgb888(src, width, height, dst);
		}
		break;

		case CALC_PIXFMT_RGB_565_LE:
		{
			ret = ticalcs_screen_convert_rgb565le_to_rgb888(src, width, height, dst);
		}
		break;

		default:
		{
			ticalcs_critical(_("Unknown pixel format %d\n"), format);
			ret = ERR_INVALID_PARAMETER;
		}
	}

	return ret;
}
