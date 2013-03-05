/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (C) 2006  Kevin Kofler
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
	TI82 support. Note: the source code is the SAME as the TI85 support (same indentation).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ticonv.h>
#include "ticalcs.h"
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"
#include "pause.h"
#include "macros.h"

#include "dbus_pkt.h"
#include "cmd82.h"
#include "rom82.h"
#include "romdump.h"

// Screen coordinates of the TI83
#define TI82_ROWS  64
#define TI82_COLS  96

static int		is_ready	(CalcHandle* handle)
{
	return 0;
}

static int		send_key	(CalcHandle* handle, uint16_t key)
{
	return 0;
}

static int		execute		(CalcHandle* handle, VarEntry *ve, const char* args)
{
	return 0;
}

static int		recv_screen	(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	uint16_t max_cnt;
	int err;
	uint8_t buf[TI82_COLS * TI82_ROWS / 8];

	sc->width = TI82_COLS;
	sc->height = TI82_ROWS;
	sc->clipped_width = TI82_COLS;
	sc->clipped_height = TI82_ROWS;

	TRYF(ti82_send_SCR(handle));
	TRYF(ti82_recv_ACK(handle, NULL));

	err = ti82_recv_XDP(handle, &max_cnt, buf);	// pb with checksum
	if (err != ERR_CHECKSUM) { TRYF(err) };
	TRYF(ti82_send_ACK(handle));

	*bitmap = (uint8_t *)g_malloc(TI82_COLS * TI82_ROWS / 8);
	if(*bitmap == NULL)	return ERR_MALLOC;
	memcpy(*bitmap, buf, TI82_COLS * TI82_ROWS / 8);

	return 0;
}

static int		get_dirlist	(CalcHandle* handle, GNode** vars, GNode** apps)
{	
	return 0;
}

static int		get_memfree	(CalcHandle* handle, uint32_t* ram, uint32_t* flash)
{
	*ram = *flash = -1;
	return 0;
}

static int		send_backup	(CalcHandle* handle, BackupContent* content)
{
  int err = 0;
  uint16_t length;
  char varname[9];
  uint8_t rej_code;
  uint16_t status;

  g_snprintf(update_->text, sizeof(update_->text), _("Waiting for user's action..."));
  update_label();

  length = content->data_length1;
  varname[0] = LSB(content->data_length2);
  varname[1] = MSB(content->data_length2);
  varname[2] = LSB(content->data_length3);
  varname[3] = MSB(content->data_length3);
  varname[4] = LSB(content->mem_address);
  varname[5] = MSB(content->mem_address);

  TRYF(ti82_send_VAR(handle, content->data_length1, TI82_BKUP, varname));
  TRYF(ti82_recv_ACK(handle, &status));

  do 
  {
	  // wait user's action
		update_refresh();

		if (update_->cancel)
		return ERR_ABORT;

		err = ti82_recv_SKP(handle, &rej_code);
  }
  while (err == ERROR_READ_TIMEOUT);

  TRYF(ti82_send_ACK(handle));
  switch (rej_code) 
  {
  case REJ_EXIT:
  case REJ_SKIP:
    return ERR_ABORT;
  case REJ_MEMORY:
    return ERR_OUT_OF_MEMORY;
  default:			// RTS
    break;
  }

  strcpy(update_->text, "");
  update_label();

  update_->cnt2 = 0;
  update_->max2 = 3;
  update_->pbar();

  TRYF(ti82_send_XDP(handle, content->data_length1, content->data_part1));
  TRYF(ti82_recv_ACK(handle, &status));
  update_->cnt2++;
  update_->pbar();

  TRYF(ti82_send_XDP(handle, content->data_length2, content->data_part2));
  TRYF(ti82_recv_ACK(handle, &status));
  update_->cnt2++;
    update_->pbar();

  TRYF(ti82_send_XDP(handle, content->data_length3, content->data_part3));
  TRYF(ti82_recv_ACK(handle, &status));
  update_->cnt2++;
    update_->pbar();

  //TRYF(ti82_send_EOT());

  return 0;
}

static int		recv_backup	(CalcHandle* handle, BackupContent* content)
{
  char varname[9] = { 0 };

  g_snprintf(update_->text, sizeof(update_->text), _("Waiting for backup..."));
  update_label();

  content->model = CALC_TI82;
  strcpy(content->comment, tifiles_comment_set_backup());

  TRYF(ti82_recv_VAR(handle, &(content->data_length1), &content->type, varname));
  content->data_length2 = (uint16_t)varname[0] | (((uint16_t)(varname[1])) << 8);
  content->data_length3 = (uint16_t)varname[2] | (((uint16_t)(varname[3])) << 8);
  content->mem_address  = (uint16_t)varname[4] | (((uint16_t)(varname[5])) << 8);
  TRYF(ti82_send_ACK(handle));

  TRYF(ti82_send_CTS(handle));
  TRYF(ti82_recv_ACK(handle, NULL));

  strcpy(update_->text, "");
  update_label();

  update_->cnt2 = 0;
  update_->max2 = 3;
  update_->pbar();

  content->data_part1 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(handle, &content->data_length1, content->data_part1));
  TRYF(ti82_send_ACK(handle));
  update_->cnt2++;
  update_->pbar();

  content->data_part2 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(handle, &content->data_length2, content->data_part2));
  TRYF(ti82_send_ACK(handle));
  update_->cnt2++;
  update_->pbar();

  content->data_part3 = tifiles_ve_alloc_data(65536);
  TRYF(ti82_recv_XDP(handle, &content->data_length3, content->data_part3));
  TRYF(ti82_send_ACK(handle));
  update_->cnt2++;
  update_->pbar();

  content->data_part4 = NULL;

  return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content);
static int		send_var	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return send_var_ns(handle, mode, content);
}

static int		recv_var	(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	return 0;
}

static int		send_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content)
{
  int i;
  int err;
  uint8_t rej_code;
  uint16_t status;
  char *utf8;

  if ((mode & MODE_SEND_EXEC_ASM) && content->num_entries != 1)
  {
    ticalcs_critical("no variable to execute");
    return -1;
  }

  update_->cnt2 = 0;
  update_->max2 = content->num_entries;

  for (i = 0; i < content->num_entries; i++) 
  {
    VarEntry *entry = content->entries[i];

    TRYF(ti82_send_VAR(handle, (uint16_t)entry->size, entry->type, entry->name));
    TRYF(ti82_recv_ACK(handle, &status));

    g_snprintf(update_->text, sizeof(update_->text), _("Waiting for user's action..."));
    update_label();

    do 
	{			// wait user's action
      update_refresh();
      if (update_->cancel)
		return ERR_ABORT;
      
	  err = ti82_recv_SKP(handle, &rej_code);
    }
    while (err == ERROR_READ_TIMEOUT);

    TRYF(ti82_send_ACK(handle));
    switch (rej_code) 
	{
    case REJ_EXIT:
      return ERR_ABORT;
    case REJ_SKIP:
      if (mode & MODE_SEND_EXEC_ASM)
        return ERR_ABORT;
      continue;
    case REJ_MEMORY:
      return ERR_OUT_OF_MEMORY;
    default:			// RTS
      break;
    }

    utf8 = ticonv_varname_to_utf8(handle->model, entry->name, entry->type);
    g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
    update_label();

    TRYF(ti82_send_XDP(handle, entry->size, entry->data));
    TRYF(ti82_recv_ACK(handle, &status));

	update_->cnt2 = i+1;
	update_->max2 = content->num_entries;
	update_->pbar();
  }

  if (mode & MODE_SEND_EXEC_ASM)
  {
    TRYF(ti82_send_asm_exec(handle, content->entries[0]));
    TRYF(ti82_recv_ERR(handle, &status));
    TRYF(ti82_send_ACK(handle));
  }
  else if ((mode & MODE_SEND_ONE_VAR) || (mode & MODE_SEND_LAST_VAR))
  {
    TRYF(ti82_send_EOT(handle));
    TRYF(ti82_recv_ACK(handle, NULL));
  }

  return 0;
}

static int		recv_var_ns	(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** vr)
{
  int nvar = 0;
  int err = 0;
  char *utf8;
  uint16_t ve_size;

  g_snprintf(update_->text, sizeof(update_->text), _("Waiting for var(s)..."));
  update_label();

  content->model = CALC_TI82;

  for (nvar = 0;; nvar++) 
  {
    VarEntry *ve;

	content->entries = tifiles_ve_resize_array(content->entries, nvar+1);
    ve = content->entries[nvar] = tifiles_ve_create();

    do 
	{
      update_refresh();
      if (update_->cancel)
		return ERR_ABORT;

      err = ti82_recv_VAR(handle, &ve_size, &(ve->type), ve->name);
      ve->size = ve_size;
    }
    while (err == ERROR_READ_TIMEOUT);

    TRYF(ti82_send_ACK(handle));
    if (err == ERR_EOT) 
	  goto exit;
    TRYF(err);

    TRYF(ti82_send_CTS(handle));
    TRYF(ti82_recv_ACK(handle, NULL));

    utf8 = ticonv_varname_to_utf8(handle->model, ve->name, ve->type);
    g_snprintf(update_->text, sizeof(update_->text), "%s", utf8);
	g_free(utf8);
    update_label();

	ve->data = tifiles_ve_alloc_data(ve->size);
    TRYF(ti82_recv_XDP(handle, &ve_size, ve->data));
    ve->size = ve_size;
    TRYF(ti82_send_ACK(handle));
  }

exit:
  content->num_entries = nvar;
  if(nvar == 1)
  {
	strcpy(content->comment, tifiles_comment_set_single());
	*vr = tifiles_ve_dup(content->entries[0]);
  }
  else
  {
	strcpy(content->comment, tifiles_comment_set_group());
	*vr = NULL;
  }

  return 0;
}

static int		send_flash	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_flash	(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	return 0;
}

static int		recv_idlist	(CalcHandle* handle, uint8_t* idlist)
{
	return 0;
}

static int		dump_rom_1	(CalcHandle* handle)
{
	// Send dumping program
	TRYF(rd_send(handle, "romdump.82p", romDumpSize82, romDump82));

	return 0;
}

static int		dump_rom_2	(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	// Get dump
	TRYF(rd_dump(handle, filename));

	return 0;
}

static int		set_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		get_clock	(CalcHandle* handle, CalcClock* _clock)
{
	return 0;
}

static int		rename_var	(CalcHandle* handle, VarRequest* oldname, VarRequest* newname)
{
	return 0;
}

static int		change_attr	(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	return 0;
}

static int		del_var		(CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		new_folder  (CalcHandle* handle, VarRequest* vr)
{
	return 0;
}

static int		get_version	(CalcHandle* handle, CalcInfos* infos)
{
	return 0;
}

static int		send_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

static int		recv_cert	(CalcHandle* handle, FlashContent* content)
{
	return 0;
}

const CalcFncts calc_82 = 
{
	CALC_TI82,
	"TI82",
	"TI-82",
	"TI-82",
	OPS_SCREEN | OPS_BACKUP | OPS_VARS | OPS_ROMDUMP | 
	FTS_BACKUP,
	{"",     /* is_ready */
	 "",     /* send_key */
	 "",     /* execute */
	 "1P",   /* recv_screen */
	 "",     /* get_dirlist */
	 "",     /* get_memfree */
	 "2P1L", /* send_backup */
	 "2P1L", /* recv_backup */
	 "",     /* send_var */
	 "",     /* recv_var */
	 "2P1L", /* send_var_ns */
	 "1P1L", /* recv_var_ns */
	 "",     /* send_app */
	 "",     /* recv_app */
	 "",     /* send_os */
	 "",     /* recv_idlist */
	 "2P",   /* dump_rom1 */
	 "2P",   /* dump_rom2 */
	 "",     /* set_clock */
	 "",     /* get_clock */
	 "",     /* del_var */
	 "",     /* new_folder */
	 "",     /* get_version */
	 "",     /* send_cert */
	 "",     /* recv_cert */
	 "",     /* rename */
	 ""      /* chattr */ },
	&is_ready,
	&send_key,
	&execute,
	&recv_screen,
	&get_dirlist,
	&get_memfree,
	&send_backup,
	&recv_backup,
	&send_var,
	&recv_var,
	&send_var_ns,
	&recv_var_ns,
	&send_flash,
	&recv_flash,
	&send_flash,
	&recv_idlist,
	&dump_rom_1,
	&dump_rom_2,
	&set_clock,
	&get_clock,
	&del_var,
	&new_folder,
	&get_version,
	&send_cert,
	&recv_cert,
	&rename_var,
	&change_attr
};
