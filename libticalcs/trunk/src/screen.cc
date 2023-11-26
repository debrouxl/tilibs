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
int TICALL ticalcs_screen_convert_bw_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < (width >> 3); j++)
		{
			const uint8_t data = *src++;
			uint8_t mask = 0x80;
			for (unsigned int bit = 0; bit < 8; bit++)
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
int TICALL ticalcs_screen_convert_bw_to_blurry_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < (width >> 3); j++)
		{
			const uint8_t data = *src++;
			uint8_t mask = 0x80;
			for (unsigned int bit = 0; bit < 8; bit++)
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
int TICALL ticalcs_screen_convert_gs4_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width / 2; j++)
		{
			const uint8_t data = *src++;
			const uint8_t hi = data >> 4;
			const uint8_t lo = data & 0x0f;

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
int TICALL ticalcs_screen_convert_rgb565le_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
{
	VALIDATE_NONNULL(src);
	VALIDATE_SCREENWIDTH(width);
	VALIDATE_SCREENHEIGHT(height);
	VALIDATE_NONNULL(dst);

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			const uint16_t data = (((uint16_t)(src[1])) << 8) | ((uint16_t)(src[0]));

			src += 2;
			*dst++ = ((data & 0xF800) >> 11) << 3;
			*dst++ = ((data & 0x07E0) >>  5) << 2;
			*dst++ = ((data & 0x001F) >>  0) << 3;
		}
	}

	return 0;
}

/**
 * ticalcs_screen_convert_native_to_rgb888:
 * @format: pixel format of the input data.
 * @src: source bitmap (16-bit pixels) to be converted.
 * @width: width of the bitmap in pixels
 * @height: height of the bitmap in pixels
 * @dst: storage space for the converted bitmap, must be at least 3 * width * height bytes large
 *
 * Convert a bitmap in the calculator's native encoding to RGB888.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_screen_convert_native_to_rgb888(CalcPixelFormat format, const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst)
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

/**
 * ticalcs_screen_nspire_rle_uncompress:
 * @format: pixel format of the input data.
 * @src: source bitmap to be converted.
 * @input_size: size of the input data.
 * @dst: storage space for the uncompressed bitmap, must be at least width * height / 2 bytes large for classic Nspire and 2 * width * height bytes large for color Nspire.
 * @max_output_size: size of the output data.
 *
 * Uncompress a RLE-compressed Nspire screenshot.
 *
 * Return value: 0 if successful, an error code otherwise.
 */
int TICALL ticalcs_screen_nspire_rle_uncompress(CalcPixelFormat format, const uint8_t *src, uint32_t input_size, uint8_t * dst, uint32_t max_output_size)
{
	int ret = 0;

	VALIDATE_NONNULL(src);
	VALIDATE_NONNULL(dst);

	switch (format)
	{
		case CALC_PIXFMT_GRAY_4:
		{
			uint8_t *q;
			uint32_t i, j;

			for (i = 0, j = 0, q = dst; i < input_size;)
			{
				const int8_t rec = src[i++];

				if (rec >= 0)
				{
					// Positive count: "repeat 8-bit value" block.
					const uint8_t cnt = ((uint8_t)rec) + 1;
					const uint8_t val = src[i++];

					if (j + cnt > max_output_size)
					{
						ret = ERR_INVALID_SCREENSHOT;
						break;
					}
					memset(q, val, cnt);
					q += cnt;
					j += cnt;
				}
				else
				{
					// Negative count: "verbatim" block of 8-bit values.
					const uint8_t cnt = ((uint8_t)-rec) + 1;

					if (j + cnt > max_output_size)
					{
						ret = ERR_INVALID_SCREENSHOT;
						break;
					}
					memcpy(q, src+i, cnt);
					q += cnt;
					i += cnt;
					j += cnt;
				}
			}
		}
		break;

		case CALC_PIXFMT_RGB_565_LE:
		{
			uint8_t *q;
			uint32_t i, j;

			for (i = 0, j = 0, q = dst; i < input_size;)
			{
				const int8_t rec = src[i++];

				if (rec >= 0)
				{
					// Positive count: "repeat 32-bit value" block.
					const uint8_t cnt = ((uint8_t)rec) + 1;
					uint32_t val;

					if (j + cnt * 4 > max_output_size)
					{
						ret = ERR_INVALID_SCREENSHOT;
						break;
					}
					memcpy(&val, src + i, sizeof(uint32_t));
					for (uint8_t k = 0; k < cnt; k++)
					{
						memcpy(q, &val, 4);
						q += 4;
					}
					i += 4;
					j += cnt * 4;
				}
				else
				{
					// Negative count: "verbatim" block of 32-bit values.
					const uint8_t cnt = ((uint8_t)-rec) + 1;

					if (j + cnt * 4 > max_output_size)
					{
						ret = ERR_INVALID_SCREENSHOT;
						break;
					}
					memcpy(q, src + i, cnt * 4);
					q += cnt * 4;
					i += cnt * 4;
					j += cnt * 4;
				}
			}
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

/**
 * ticalcs_screen_84pcse_rle_uncompress:
 * @src: source bitmap to be converted.
 * @src_length: size of the input data.
 * @dst: storage space for the uncompressed bitmap, must be at least width * height / 2 bytes large for classic Nspire and 2 * width * height bytes large for color Nspire.
 * @dst_length: size of the output data.
 *
 * Uncompress a RLE-compressed 84+CSE screenshot.
 *
 * Return value: 0 if successful, an error code otherwise.
 */
int TICALL ticalcs_screen_84pcse_rle_uncompress(const uint8_t *src, uint32_t src_length, uint8_t *dst, uint32_t dst_length)
{
	unsigned int i, c, n;

	VALIDATE_NONNULL(src);
	VALIDATE_NONNULL(dst);

	if (src[0] != 1)
	{
		return ERR_INVALID_SCREENSHOT;
	}
	src++;
	src_length--;

	const unsigned int palette_size = src[src_length - 1];
	if (src_length <= palette_size * 2 + 1)
	{
		return ERR_INVALID_SCREENSHOT;
	}

	src_length -= palette_size * 2 + 1;
	const uint8_t* palette = src + src_length - 2;

	while (src_length > 0)
	{
		if ((src[0] & 0xf0) != 0)
		{
			for (i = 0; i < 2; i ++)
			{
				c = (i == 0 ? src[0] >> 4 : src[0] & 0x0f);
				if (c == 0)
				{
					break;
				}

				if (c > palette_size)
				{
					return ERR_INVALID_SCREENSHOT;
				}

				if (dst_length < 2)
				{
					return ERR_INVALID_SCREENSHOT;
				}

				dst[0] = palette[2 * c];
				dst[1] = palette[2 * c + 1];
				dst += 2;
				dst_length -= 2;
			}
			src++;
			src_length--;
		}
		else if (src_length >= 2 && (src[0] & 0x0f) != 0)
		{
			c = src[0];
			n = src[1];

			if (c > palette_size)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			if (dst_length < 2 * n)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			for (i = 0; i < n; i++)
			{
				dst[0] = palette[2 * c];
				dst[1] = palette[2 * c + 1];
				dst += 2;
				dst_length -= 2;
			}

			src += 2;
			src_length -= 2;
		}
		else if (src_length >= 2 && src[0] == 0 && src[1] == 0)
		{
			src += 2;
			src_length -= 2;
			goto byte_mode;
		}
		else
		{
			return ERR_INVALID_SCREENSHOT;
		}
	}
	goto finish;

byte_mode:
	while (src_length > 0)
	{
		if (src[0] != 0)
		{
			c = src[0];

			if (c > palette_size)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			if (dst_length < 2)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			dst[0] = palette[2 * c];
			dst[1] = palette[2 * c + 1];
			dst += 2;
			dst_length -= 2;

			src++;
			src_length--;
		}
		else if (src_length >= 3 && src[1] != 0)
		{
			c = src[1];
			n = src[2];

			if (c > palette_size)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			if (dst_length < 2 * n)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			for (i = 0; i < n; i++)
			{
				dst[0] = palette[2 * c];
				dst[1] = palette[2 * c + 1];
				dst += 2;
				dst_length -= 2;
			}

			src += 3;
			src_length -= 3;
		}
		else if (src_length >= 3 && src[0] == 0 && src[1] == 0 && src[2] == 0)
		{
			src += 3;
			src_length -= 3;
			goto word_mode;
		}
		else
		{
			return ERR_INVALID_SCREENSHOT;
		}
	}
	goto finish;

word_mode:
	while (src_length > 0)
	{
		if (src_length < 2)
		{
			return ERR_INVALID_SCREENSHOT;
		}

		if (src[0] != 0x01 || src[1] != 0x00)
		{
			if (dst_length < 2)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			dst[0] = src[0];
			dst[1] = src[1];
			dst += 2;
			dst_length -= 2;
			src += 2;
			src_length -= 2;
		}
		else
		{
			if (src_length < 5)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			n = src[4];

			if (dst_length < 2 * n)
			{
				return ERR_INVALID_SCREENSHOT;
			}

			for (i = 0; i < n; i++)
			{
				dst[0] = src[2];
				dst[1] = src[3];
				dst += 2;
				dst_length -= 2;
			}

			src += 5;
			src_length -= 5;
		}
	}

finish:
	if (src_length != 0 || dst_length != 0)
	{
		return ERR_INVALID_SCREENSHOT;
	}
	else
	{
		return 0;
	}
}
