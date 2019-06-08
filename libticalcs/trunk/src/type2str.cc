/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
 * ticalcs_model_to_string:
 * @model: a calculator model.
 *
 * Do an integer to string conversion.
 *
 * Return value: a string like "TI92+".
 **/
const char *TICALL ticalcs_model_to_string(CalcModel model)
{
	return tifiles_model_to_string(model);
}

/**
 * ticalcs_string_to_model:
 * @str: a calculator model as string like "TI92".
 *
 * Do a string to integer conversion.
 *
 * Return value: a calculator model.
 **/
CalcModel TICALL ticalcs_string_to_model(const char *str)
{
	return tifiles_string_to_model(str);
}


const char *TICALL ticalcs_scrfmt_to_string(CalcScreenFormat format)
{
	switch (format)
	{
	case SCREEN_FULL: return _("full");
	case SCREEN_CLIPPED: return _("clipped");
	default: return _("unknown");
	}
}

CalcScreenFormat TICALL ticalcs_string_to_scrfmt(const char *str)
{
	if (str != NULL)
	{
		if (!strcmp(str, _("full")))
		{
			return SCREEN_FULL;
		}
		else if (!strcmp(str, _("clipped")))
		{
			return SCREEN_CLIPPED;
		}
	}
	else
	{
		ticalcs_critical("ticalcs_string_to_scrfmt(NULL)");
	}

	return SCREEN_CLIPPED;
}


const char *TICALL ticalcs_pathtype_to_string(CalcPathType type)
{
	switch (type)
	{
	case PATH_FULL: return _("full");
	case PATH_LOCAL: return _("local");
	default: return _("unknown");
	}
}

CalcPathType TICALL ticalcs_string_to_pathtype(const char *str)
{
	if (str != NULL)
	{
		if (!strcmp(str, _("full")))
		{
			return PATH_FULL;
		}
		else if (!strcmp(str, _("local")))
		{
			return PATH_LOCAL;
		}
	}
	else
	{
		ticalcs_critical("ticalcs_string_to_pathtype(NULL)");
	}

	return PATH_FULL;
}


const char *TICALL ticalcs_memtype_to_string(CalcMemType type)
{
	switch (type)
	{
	case MEMORY_FREE: return _("free");
	case MEMORY_USED: return _("used");
	default: return _("unknown");
	}
}

CalcMemType TICALL ticalcs_string_to_memtype(const char *str)
{
	if (str != NULL)
	{
		if (!strcmp(str, _("free")))
		{
			return MEMORY_FREE;
		}
		else if (!strcmp(str, _("used")))
		{
			return MEMORY_USED;
		}
	}

	return MEMORY_NONE;
}

int TICALL ticalcs_infos_to_string(CalcInfos *infos, char *str, uint32_t maxlen)
{
	if (infos != NULL)
	{
		char language_ids[25];
		char device_type[6];
		char hw_version[11];
		char clock_speed[15];
		char lcd_width[20];
		char lcd_height[20];
		char bpp[20];
		char color_screen[11];
		char ram_phys[50];
		char ram_user[50];
		char ram_free[50];
		char flash_phys[50];
		char flash_user[50];
		char flash_free[50];

		language_ids[0] = 0;
		if (infos->mask & INFOS_LANG_ID)
		{
			sprintf(language_ids, "%d %d", infos->language_id, infos->sub_lang_id);
		}

		device_type[0] = 0;
		if (infos->mask & INFOS_DEVICE_TYPE)
		{
			sprintf(device_type, "%02X", infos->device_type);
		}
		hw_version[0] = 0;
		if (infos->mask & INFOS_HW_VERSION)
		{
			sprintf(hw_version, "%d", infos->hw_version);
		}

		clock_speed[0] = 0;
		if (infos->mask & INFOS_CLOCK_SPEED)
		{
			sprintf(clock_speed, "%d MHz", infos->clock_speed);
		}

		lcd_width[0] = 0;
		if (infos->mask & INFOS_LCD_WIDTH)
		{
			sprintf(lcd_width, "%d pixels", infos->lcd_width);
		}
		lcd_height[0] = 0;
		if (infos->mask & INFOS_LCD_HEIGHT)
		{
			sprintf(lcd_height, "%d pixels", infos->lcd_height);
		}
		bpp[0] = 0;
		if (infos->mask & INFOS_BPP)
		{
			sprintf(bpp, "%d pixels", infos->bits_per_pixel);
		}
		color_screen[0] = 0;
		if (infos->mask & INFOS_COLOR_SCREEN)
		{
			sprintf(color_screen, "%d", infos->color_screen);
		}

		ram_phys[0] = 0; // The casts below aren't truncating in practice until one of the models grows more than 4 GB of RAM or Flash.
		if (infos->mask & INFOS_RAM_PHYS)
		{
			sprintf(ram_phys, "%lu B (%lu KB)", (unsigned long)infos->ram_phys, (unsigned long)((infos->ram_phys + 512) >> 10));
		}
		ram_user[0] = 0;
		if (infos->mask & INFOS_RAM_USER)
		{
			sprintf(ram_user, "%lu B (%lu KB)", (unsigned long)infos->ram_user, (unsigned long)((infos->ram_user + 512) >> 10));
		}
		ram_free[0] = 0;
		if (infos->mask & INFOS_RAM_FREE)
		{
			sprintf(ram_free, "%lu B (%lu KB)", (unsigned long)infos->ram_free, (unsigned long)((infos->ram_free + 512) >> 10));
		}
		flash_phys[0] = 0;
		if (infos->mask & INFOS_FLASH_PHYS)
		{
			sprintf(flash_phys, "%lu B (%lu KB)", (unsigned long)infos->flash_phys, (unsigned long)((infos->flash_phys + 512) >> 10));
		}
		flash_user[0] = 0;
		if (infos->mask & INFOS_FLASH_USER)
		{
			sprintf(flash_user, "%lu B (%lu KB)", (unsigned long)infos->flash_user, (unsigned long)((infos->flash_user + 512) >> 10));
		}
		flash_free[0] = 0;
		if (infos->mask & INFOS_FLASH_FREE)
		{
			sprintf(flash_free, "%lu B (%lu KB)", (unsigned long)infos->flash_free, (unsigned long)((infos->flash_free + 512) >> 10));
		}

		ticalcs_slprintf(str, maxlen,
			"%s"   // INFOS_PRODUCT_NAME
			"%s\n"
			"%s"   // INFOS_PRODUCT_ID
			"%s\n"
			"%s"   // INFOS_LANG_ID
			"%s\n"
			"%s"   // INFOS_USER_DEFINED_ID
			"%s\n"
			"\n"
			"%s"   // INFOS_DEVICE_TYPE
			"%s\n"
			"%s"   // INFOS_HW_VERSION
			"%s\n"
			"%s"   // INFOS_BOOT_VERSION
			"%s\n"
			"%s"   // INFOS_BOOT2_VERSION
			"%s\n"
			"%s"   // INFOS_OS_VERSION
			"%s\n"
			"%s"   // INFOS_RUN_LEVEL
			"%s\n"
			"%s"   // INFOS_CLOCK_SPEED
			"%s\n"
			"%s"   // INFOS_MATH_CAPABILITIES
			"%s\n"
			"%s"   // INFOS_PYTHON_ON_BOARD
			"%s\n"
			"\n"
			"%s"   // INFOS_LCD_WIDTH
			"%s\n"
			"%s"   // INFOS_LCD_HEIGHT
			"%s\n"
			"%s"   // INFOS_BPP
			"%s\n"
			"%s"   // INFOS_COLOR_SCREEN
			"%s\n"
			"\n"
			"%s"   // INFOS_RAM_PHYS
			"%s\n"
			"%s"   // INFOS_RAM_USER
			"%s\n"
			"%s"   // INFOS_RAM_FREE
			"%s\n"
			"%s"   // INFOS_FLASH_PHYS
			"%s\n"
			"%s"   // INFOS_FLASH_USER
			"%s\n"
			"%s"   // INFOS_FLASH_FREE
			"%s\n"
			"\n"
			"%s"   // INFOS_BATTERY_ENOUGH
			"%s\n",

			(infos->mask & INFOS_PRODUCT_NAME) ? _("Product Name: ") : "",
			(infos->mask & INFOS_PRODUCT_NAME) ? infos->product_name : "",
			(infos->mask & INFOS_PRODUCT_ID) ? _("Product ID: ") : "",
			(infos->mask & INFOS_PRODUCT_ID) ? infos->product_id : "",
			(infos->mask & INFOS_LANG_ID) ? _("Language ID: ") : "",
			language_ids,
			(infos->mask & INFOS_USER_DEFINED_ID) ? _("User-defined ID: ") : "",
			(infos->mask & INFOS_USER_DEFINED_ID) ? infos->user_defined_id : "",

			(infos->mask & INFOS_DEVICE_TYPE) ? _("Device Type: ") : "",
			device_type,
			(infos->mask & INFOS_HW_VERSION) ? _("Hardware Version: ") : "",
			hw_version,

			(infos->mask & INFOS_BOOT_VERSION) ? _("Boot Version: ") : "",
			(infos->mask & INFOS_BOOT_VERSION) ? infos->boot_version : "",
			(infos->mask & INFOS_BOOT2_VERSION) ? _("Boot2 Version: ") : "",
			(infos->mask & INFOS_BOOT2_VERSION) ? infos->boot2_version : "",
			(infos->mask & INFOS_OS_VERSION) ? _("OS Version: ") : "",
			(infos->mask & INFOS_OS_VERSION) ? infos->os_version : "",

			(infos->mask & INFOS_RUN_LEVEL) ? _("Run level: ") : "",
			(infos->mask & INFOS_RUN_LEVEL) ? ((infos->run_level == 2) ? "OS" : "boot") : "",
			(infos->mask & INFOS_CLOCK_SPEED) ? _("Clock speed: ") : "",
			clock_speed,
			(infos->mask & INFOS_MATH_CAPABILITIES) ? _("Math capabilities (exact math engine): ") : "",
			(infos->mask & INFOS_MATH_CAPABILITIES) ? ((infos->exact_math) ? _("Yes") : _("No")) : "",
			(infos->mask & INFOS_PYTHON_ON_BOARD) ? _("Python on board: ") : "",
			(infos->mask & INFOS_PYTHON_ON_BOARD) ? ((infos->python_on_board) ? _("Yes") : _("No")) : "",

			(infos->mask & INFOS_LCD_WIDTH) ? _("LCD width: ") : "",
			lcd_width,
			(infos->mask & INFOS_LCD_HEIGHT) ? _("LCD height: ") : "",
			lcd_height,
			(infos->mask & INFOS_BPP) ? _("Bits per pixel: ") : "",
			bpp,
			(infos->mask & INFOS_COLOR_SCREEN) ? _("Color screen: ") : "",
			color_screen,

			(infos->mask & INFOS_RAM_PHYS) ? _("Physical RAM: ") : "",
			ram_phys,
			(infos->mask & INFOS_RAM_USER) ? _("User RAM: ") : "",
			ram_user,
			(infos->mask & INFOS_RAM_FREE) ? _("Free RAM: ") : "",
			ram_free,
			(infos->mask & INFOS_FLASH_PHYS) ? _("Physical Flash: ") : "",
			flash_phys,
			(infos->mask & INFOS_FLASH_USER) ? _("User Flash: ") : "",
			flash_user,
			(infos->mask & INFOS_FLASH_FREE) ? _("Free Flash: ") : "",
			flash_free,

			(infos->mask & INFOS_BATTERY_ENOUGH) ? _("Battery: ") : "",
			(infos->mask & INFOS_BATTERY_ENOUGH) ? (infos->battery ? _("good") : _("low")) : "");
		return 0;
	}
	else
	{
		ticalcs_critical("%s: infos is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
}
