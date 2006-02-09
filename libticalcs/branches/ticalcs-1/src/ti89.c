/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (c) 2005, Christian Walther (patches for Mac OS-X port)
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  This unit provides TI89/TI92+/V200/TI89 Titanium support.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "gettext.h"

#include "headers.h"
#include "externs.h"
#include "update.h"
#include "packets.h"
#include "calc_err.h"
#include "cmd89.h"
#include "rom89.h"
#include "keys89.h"
#include "pause.h"
#include "dirlist.h"
#include "printl.h"

// Screen coordinates of the TI89
#define TI89_ROWS          128
#define TI89_COLS          240
#define TI89_ROWS_VISIBLE  100
#define TI89_COLS_VISIBLE  160

int ti89_supported_operations(void)
{
  return
      (OPS_ISREADY |
       OPS_SCREENDUMP |
       OPS_SEND_KEY | OPS_RECV_KEY | OPS_REMOTE |
       OPS_DIRLIST |
       OPS_SEND_BACKUP | OPS_RECV_BACKUP |
       OPS_SEND_VARS | OPS_RECV_VARS |
       OPS_SEND_FLASH | OPS_RECV_FLASH |
       OPS_IDLIST | OPS_ROMDUMP | OPS_CLOCK);
}

int ti89_isready(void)
{
  uint16_t status;

  printl2(0, _("Is calculator ready ?\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  TRYF(ti89_send_RDY());
  TRYF(ti89_recv_ACK(&status));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return (status & 0x01) ? ERR_NOT_READY : 0;
}

int ti89_send_key(uint16_t key)
{
  LOCK_TRANSFER();
  TRYF(cable->open());

  TRYF(ti89_send_KEY(key));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti89_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc)
{
  uint32_t max_cnt;
  int err;

  printl2(0, _("Receiving screendump...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sc->width = TI89_COLS;
  sc->height = TI89_ROWS;
  switch (ticalcs_calc_type) {
  case CALC_TI89:
  case CALC_TI89T:
    sc->clipped_width = TI89_COLS_VISIBLE;
    sc->clipped_height = TI89_ROWS_VISIBLE;
    break;
  case CALC_TI92P:
  case CALC_V200:
    sc->clipped_width = TI89_COLS;
    sc->clipped_height = TI89_ROWS;
    break;
  }

  if (*bitmap != NULL)
    free(*bitmap);
  (*bitmap) =
      (uint8_t *) malloc(TI89_COLS * TI89_ROWS * sizeof(uint8_t) / 8);
  if ((*bitmap) == NULL) {
    printl2(2, "Unable to allocate memory.\n");
    exit(0);
  }

  TRYF(ti89_send_SCR());
  TRYF(ti89_recv_ACK(NULL));

  err = ti89_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
  if (err != ERR_CHECKSUM) {
  TRYF(err)};
  TRYF(ti89_send_ACK());

  printl2(0, _("Done.\n"));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  // Clip the unused part of the screen (nethertheless useable witha asm prog)
  if (((ticalcs_calc_type == CALC_TI89) || (ticalcs_calc_type == CALC_TI89T))
      && (mask_mode == CLIPPED_SCREEN)) {
    int i, j, k;

    for (i = 0, j = 0; j < TI89_ROWS_VISIBLE; j++)
      for (k = 0; k < (TI89_COLS_VISIBLE >> 3); k++)
	(*bitmap)[i++] = (*bitmap)[j * (TI89_COLS >> 3) + k];
  }

  return 0;
}

int ti89_directorylist(TNode ** tree, uint32_t * memory)
{
  uint8_t buffer[65536];
  TiVarEntry info;
  uint32_t block_size;
  int i, j;
  TNode *vars, *apps;
  uint8_t extra = (ticalcs_calc_type == CALC_V200) ? 8 : 0;

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  printl2(0, _("Directory listing...\n"));

  TRYF(ti89_send_REQ(TI89_FDIR << 24, TI89_RDIR, ""));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_VAR(&info.size, &info.type, info.name));
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_CTS());
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_XDP(&block_size, buffer));
  TRYF(ti89_send_ACK());

  TRYF(ti89_recv_EOT());
  TRYF(ti89_send_ACK());

  // get list of folders & FLASH apps
  *tree = t_node_new(NULL);
  vars = t_node_new(NULL);
  apps = t_node_new(NULL);
  t_node_append(*tree, vars);
  t_node_append(*tree, apps);

  for (j = 4; j < (int)block_size;) {
    TiVarEntry *fe = calloc(1, sizeof(TiVarEntry));
    TNode *node;

    memcpy(fe->name, buffer + j, 8);
    fe->name[8] = '\0';
    fe->type = buffer[j + 8];
    fe->attr = buffer[j + 9];
    fe->size = buffer[j + 10] | (buffer[j + 11] << 8) | (buffer[j + 12] << 16);	// | (buffer[j+13] << 24);
    j += 14 + extra;
    strcpy(fe->folder, "");

    tifiles_translate_varname(fe->name, fe->trans, fe->type);
    node = t_node_new(fe);

    printl2(0, _("Name: %8s | "), fe->name);
    printl2(0, _("Type: %8s | "), tifiles_vartype2string(fe->type));
    printl2(0, _("Attr: %i  | "), fe->attr);
    printl2(0, _("Size: %08X\n"), fe->size);

    if (fe->type == TI89_DIR)
      t_node_append(vars, node);
    else if (fe->type == TI89_APPL)
      continue;			// AMS<2.08 returns FLASH apps
  }

  // get list of variables into each folder
  for (i = 0; i < (int)t_node_n_children(vars); i++) {
    TNode *folder = t_node_nth_child(vars, i);
    char *folder_name = ((TiVarEntry *) (folder->data))->name;

    printl2(0, _("Directory listing in %8s...\n"), folder_name);

    TRYF(ti89_send_REQ(TI89_LDIR << 24, TI89_RDIR, folder_name));
    TRYF(ti89_recv_ACK(NULL));

    TRYF(ti89_recv_VAR(&info.size, &info.type, info.name));
    TRYF(ti89_send_ACK());

    TRYF(ti89_send_CTS());
    TRYF(ti89_recv_ACK(NULL));

    TRYF(ti89_recv_XDP(&block_size, buffer));
    TRYF(ti89_send_ACK());

    TRYF(ti89_recv_EOT());
    TRYF(ti89_send_ACK());

    for (j = 4 + 14 + extra; j < (int)block_size;) {
      TiVarEntry *ve = calloc(1, sizeof(TiVarEntry));
      TNode *node;

      memcpy(ve->name, buffer + j, 8);
      ve->name[8] = '\0';
      ve->type = buffer[j + 8];
      ve->attr = buffer[j + 9];
      ve->size = buffer[j + 10] | (buffer[j + 11] << 8) | (buffer[j + 12] << 16);	// | (buffer[j+13] << 24);
      j += 14 + extra;
      strcpy(ve->folder, folder_name);

      tifiles_translate_varname(ve->name, ve->trans, ve->type);
      node = t_node_new(ve);

      printl2(0, _("Name: %8s | "), ve->trans);
      printl2(0, _("Type: %8s | "), tifiles_vartype2string(ve->type));
      printl2(0, _("Attr: %i  | "), ve->attr);
      printl2(0, _("Size: %08X\n"), ve->size);

      sprintf(update->label_text, _("Reading of '%s/%s'"),
	      ((TiVarEntry *) (folder->data))->trans, ve->trans);
      update_label();
      if (update->cancel)
	return -1;

      if (ve->type == TI89_APPL) {
	if (!ticalc_check_if_app_exists(*tree, ve->name))
	  t_node_append(apps, node);
	else
	  free(ve);
      } else
	t_node_append(folder, node);
    }
    printl2(0, "\n");
  }

  *memory = ticalc_dirlist_memused(*tree);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti89_recv_var(char *filename, int mask_mode, TiVarEntry * entry);

int ti89_recv_backup(const char *filename, int mask_mode)
{
  int i, j;
  int i_max, j_max;
  int mask = MODE_BACKUP;
  TNode *tree, *vars;
  uint32_t mem;
  int nvars, ivars = 0;
  int b;

  printl2(0, _("Receiving backup...\n"));

  // Do a directory list and check for something to backup
  TRYF(ti89_directorylist(&tree, &mem));
  nvars = ticalc_dirlist_numvars(tree);
  if (!nvars)
    return ERR_NO_VARS;

  // Check whether the last folder is empty
  vars = t_node_nth_child(tree, 0);
  b = t_node_n_children(t_node_nth_child(vars,
					 t_node_n_children(vars) - 1));

  // Receive all variables, except FLASH apps
  i_max = t_node_n_children(vars);
  for (i = 0; i < i_max; i++) {
    TNode *parent = t_node_nth_child(vars, i);

    j_max = t_node_n_children(parent);
    for (j = 0; j < j_max; j++) {
      TNode *node = t_node_nth_child(parent, j);

      TiVarEntry *ve = (TiVarEntry *) (node->data);

      if (!i && !j)
	mask = mask_mode | MODE_RECEIVE_FIRST_VAR;
      else if ((i == i_max - 1) && (j == j_max - 1) && b)
	mask = mask_mode | MODE_RECEIVE_LAST_VAR;
      else if ((i == i_max - 2) && (j == j_max - 1) && !b)
	mask = mask_mode | MODE_RECEIVE_LAST_VAR;
      else
	mask = mask_mode;

      TRYF(ti89_isready());
      TRYF(ti89_recv_var((char *) filename, mask, ve));

      (update->main_percentage) = (float) ivars++ / nvars;
      if (update->cancel)
	return ERR_ABORT;
    }
  }

  ticalc_dirlist_destroy(&tree);

  update_stop();
  TRY(cable->close());

  return 0;
}

int ti89_send_var(const char *filename, int mask_mode, char **actions);

int ti89_send_backup(const char *filename, int mask_mode)
{
  printl2(0, _("Sending backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending backup..."));
  update_label();

  TRYF(ti89_send_VAR(0, TI89_BKUP, "main"));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_CTS());
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_EOT());
  TRYF(ti89_recv_ACK(NULL));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  TRYF(ti89_send_var(filename, mask_mode | MODE_BACKUP, NULL));

  return 0;
}

int ti89_recv_var(char *filename, int mask_mode, TiVarEntry * entry)
{
  static Ti9xRegular *content;
  uint16_t status;
  TiVarEntry *ve;
  char *fn;
  static int nvar = 0;
  uint32_t unused;
  uint8_t varname[20], utf8[35];

  printl2(0, _("Receiving variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  if ((mask_mode & MODE_RECEIVE_FIRST_VAR) ||
      (mask_mode & MODE_RECEIVE_SINGLE_VAR)) {
    content = ti9x_create_regular_content();
    nvar = 0;
  }

  content->calc_type = ticalcs_calc_type;
  content->entries = (TiVarEntry *) tifiles_realloc(content->entries,
						    (nvar +
						     1) *
						    sizeof(TiVarEntry));
  ve = &(content->entries[nvar]);
  memcpy(ve, entry, sizeof(TiVarEntry));

  strcpy((char*)varname, entry->folder);
  strcat((char*)varname, "\\");
  strcat((char*)varname, entry->name);

  tifiles_translate_varname((char*)varname, (char*)utf8, entry->type);
  sprintf(update->label_text, _("Receiving '%s'"), utf8);
  update_label();

  TRYF(ti89_send_REQ(0, entry->type, (char*)varname));
  TRYF(ti89_recv_ACK(&status));
  if (status != 0)
    return ERR_MISSING_VAR;

  TRYF(ti89_recv_VAR(&ve->size, &ve->type, ve->name));
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_CTS());
  TRYF(ti89_recv_ACK(NULL));

  ve->data = tifiles_calloc(ve->size + 4, 1);
  TRYF(ti89_recv_XDP(&unused, ve->data));
  memmove(ve->data, ve->data + 4, ve->size);
  TRYF(ti89_send_ACK());

  TRYF(ti89_recv_EOT());
  TRYF(ti89_send_ACK());

  if (++nvar > 1)
    strcpy(content->comment, "Group file received by TiLP");
  else
    strcpy(content->comment, "Single file received by TiLP");

  content->num_entries = nvar;
  if (mask_mode & MODE_RECEIVE_SINGLE_VAR) {	// single
    ti9x_write_regular_file(NULL, content, &fn);
    strcpy(filename, fn);
    tifiles_free(fn);
    ti9x_free_regular_content(content);
  } else if (mask_mode & MODE_RECEIVE_LAST_VAR) {	// group
    ti9x_write_regular_file(filename, content, NULL);
    ti9x_free_regular_content(content);
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  PAUSE(PAUSE_BETWEEN_VARS);

  return 0;
}

int ti89_send_var(const char *filename, int mask_mode, char **actions)
{
  Ti9xRegular content = { 0 };
  int i;
  uint16_t status;

  printl2(0, _("Sending variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending variable(s)..."));
  update_label();

  TRYF(ti9x_read_regular_file(filename, &content));

  for (i = 0; i < content.num_entries; i++) {
    TiVarEntry *entry = &(content.entries[i]);
    uint8_t buffer[65536 + 4] = { 0 };
    uint8_t vartype = entry->type;
    uint8_t full_name[18], varname[18], utf8[35];

    if (actions == NULL)	// backup or old behaviour
      strcpy((char*)varname, entry->name);
    else if (actions[i][0] == ACT_SKIP) {
      printl2(0, _(" '%s' has been skipped !\n"), entry->name);
      continue;
    } else if (actions[i][0] == ACT_OVER)
      strcpy((char*)varname, actions[i] + 1);

    if ((mask_mode & MODE_LOCAL_PATH) && !(mask_mode & MODE_BACKUP)) {	// local & not backup
      strcpy((char*)full_name, (char*)varname);
    } else {			// full or backup
      strcpy((char*)full_name, entry->folder);
      strcat((char*)full_name, "\\");
      strcat((char*)full_name, (char*)varname);
    }

    tifiles_translate_varname((char*)full_name, (char*)utf8, entry->type);
    sprintf(update->label_text, _("Sending '%s'"), utf8);
    update_label();

    if (mask_mode & MODE_BACKUP) {	// backup: keep attributes
      switch (entry->attr) {
      case TI89_VNONE:
	vartype = TI89_BKUP;
	break;
      case TI89_VLOCK:
	vartype = 0x26;
	break;
      case TI89_VARCH:
	vartype = 0x27;
	break;
      }
      TRYF(ti89_send_RTS(entry->size, vartype, (char*)full_name));
    } else {
      TRYF(ti89_send_VAR(entry->size, vartype, (char*)full_name));
    }

    TRYF(ti89_recv_ACK(NULL));

    TRYF(ti89_recv_CTS());
    TRYF(ti89_send_ACK());

    memcpy(buffer + 4, entry->data, entry->size);
    TRYF(ti89_send_XDP(entry->size + 4, buffer));
    TRYF(ti89_recv_ACK(&status));

    TRYF(ti89_send_EOT());
    TRYF(ti89_recv_ACK(NULL));

    if (mask_mode & MODE_BACKUP) {
      (update->main_percentage) = (float) i / content.num_entries;

    if (update->cancel)
	        return ERR_ABORT;
    }
  }

  ti9x_free_regular_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

// automatically build filename and put it into 'filename'.
// mask_mode is unused
// 'entry' contains informations
int ti89_recv_var_2(char *filename, int mask_mode, TiVarEntry * entry)
{
	Ti9xRegular *content;
	uint32_t unused;
	uint8_t utf8[35];
	int nvar, err;
    char tipath[18];
    char *tiname;

	printl2(0, _("Receiving variable(s)...\n"));

	// open cable
	LOCK_TRANSFER();
	TRYF(cable->open());
	update_start();

	// create variable content and fills
    content = ti9x_create_regular_content();
	content->calc_type = ticalcs_calc_type;

	// receive packets
	for(nvar = 1;; nvar++)
	{
		TiVarEntry *ve;

		content->entries = (TiVarEntry *) tifiles_realloc(content->entries, nvar * sizeof(TiVarEntry));
		ve = &(content->entries[nvar-1]);
		strcpy(ve->folder, "main");	

		err = ti89_recv_VAR(&ve->size, &ve->type, tipath);
		TRYF(ti89_send_ACK());

		if(err == ERR_EOT)	// end of transmission
			goto exit;
		else
			content->num_entries = nvar;

		// from Christian (TI can send varname or fldname/varname)
        if ((tiname = strchr(tipath, '\\')) != NULL) 
		{
			*tiname = '\0';
            strcpy(ve->folder, tipath);
            strcpy(ve->name, tiname + 1);
        }
        else 
		{
            strcpy(ve->folder, "main");
            strcpy(ve->name, tipath);
        }

		tifiles_translate_varname(ve->name, (char*)utf8, ve->type);
		sprintf(update->label_text, _("Receiving '%s'"), utf8);
		update_label();

		TRYF(ti89_send_CTS());
		TRYF(ti89_recv_ACK(NULL));

		ve->data = tifiles_calloc(ve->size + 4, 1);
		TRYF(ti89_recv_XDP(&unused, ve->data));
		memmove(ve->data, ve->data + 4, ve->size);
		TRYF(ti89_send_ACK());
	}

exit:
	// close cable
	TRYF(cable->close());
	UNLOCK_TRANSFER();
    
	// write file content
	nvar--;
	if(nvar > 1)
	{
		strcpy(content->comment, "Group file received by TiLP");
		strcat(filename, "group.92g");
		ti9x_write_regular_file(filename, content, NULL);
	}
	else
	{
		strcpy(content->comment, "Single file received by TiLP");
		strcat(filename, content->entries[0].name);
		strcat(filename, ".");
		strcat(filename, tifiles_vartype2file(content->entries[0].type));
		ti9x_write_regular_file(filename, content, NULL);
	}
    ti9x_free_regular_content(content);

	return 0;
}

int ti89_send_flash(const char *filename, int mask_mode)
{
  Ti9xFlash content = { 0 };
  Ti9xFlash *ptr;
  int i, nblocks;
  int nheaders = 0;

  printl2(0, _("Sending FLASH app/os...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending FLASH OS/App..."));
  update_label();

  TRYF(ti9x_read_flash_file(filename, &content));

  // count headers
  for (ptr = &content; ptr != NULL; ptr = ptr->next)
    nheaders++;

  // keep the last one (data)
  for (i = 0, ptr = &content; i < nheaders - 1; i++)
    ptr = ptr->next;

  printl2(0, _("FLASH app/os name: \"%s\"\n"), ptr->name);
  printl2(0, _("FLASH app/os size: %i bytes.\n"), ptr->data_length);

  if (ptr->data_type == TI89_AMS) 
  {
	  if(ticalcs_calc_type == CALC_TI89T)
	  {
		TRYF(ti89_send_RTS2(ptr->data_length, ptr->data_type, ""));
	  }
	  else
	  {
		TRYF(ti89_send_RTS(ptr->data_length, ptr->data_type, ""));
	  }
  } 
  else 
  {
    TRYF(ti89_send_RTS(ptr->data_length, ptr->data_type, ptr->name));
  }

  nblocks = ptr->data_length / 65536;
  for (i = 0; i <= nblocks; i++) {
    uint32_t length = (i != nblocks) ? 65536 : ptr->data_length % 65536;

    TRYF(ti89_recv_ACK(NULL));

    TRYF(ti89_recv_CTS());
    TRYF(ti89_send_ACK());

    TRYF(ti89_send_XDP(length, (ptr->data_part) + 65536 * i));
    TRYF(ti89_recv_ACK(NULL));

    if (i != nblocks) {
      TRYF(ti89_send_CONT());
    } else {
      TRYF(ti89_send_EOT());
    }

    update->main_percentage = (float) i / nblocks;
    if (update->cancel)
	return ERR_ABORT;
  }

  TRYF(ti89_recv_ACK(NULL));

  printl2(0, _("Flash application/os sent completely.\n"));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti89_recv_flash(const char *filename, int mask_mode, TiVarEntry * ve)
{
  Ti9xFlash *content;
  int i;

  printl2(0, _("Receiving FLASH application...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  content = ti9x_create_flash_content();
  content->calc_type = ticalcs_calc_type;
  content->data_part = (uint8_t *) tifiles_calloc(2 * 1024 * 1024, 1);	// 2MB max

  sprintf(update->label_text, _("Receiving '%s'"), ve->name);
  update_label();

  TRYF(ti89_send_REQ(0x00, TI89_APPL, ve->name));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_VAR(&content->data_length, &content->data_type,
		     content->name));

  content->data_length = 0;
  for (i = 0;; i++) {
    int err;
    uint32_t block_size;

    TRYF(ti89_send_ACK());

    TRYF(ti89_send_CTS());
    TRYF(ti89_recv_ACK(NULL));

    TRYF(ti89_recv_XDP
	 (&block_size, content->data_part + content->data_length));
    TRYF(ti89_send_ACK());

    content->data_length += block_size;

    err = ti89_recv_CONT();
    if (err == ERR_EOT)
      break;
    TRYF(err);

    (update->main_percentage) = (float) content->data_length / ve->size;
    if (update->cancel)
      return ERR_ABORT;
  }

  TRYF(ti89_send_ACK());

  ti9x_write_flash_file(filename, content);
  ti9x_free_flash_content(content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

#define DUMP_ROM89_FILE "dumprom.89z"
//#define ROMSIZE (2*1024)	// 2MB or 4MB (Titanium)

int ti89_dump_rom(const char *filename, int mask_mode)
{
  int i, j;
  uint8_t data;
  time_t start, elapsed, estimated, remaining;
  char buffer[257];
  char tmp[257];
  int pad;
  FILE *f, *file;
  uint16_t checksum, sum;
  int ROMSIZE = (ticalcs_calc_type == CALC_TI89T) || 
  				(ticalcs_calc_type == CALC_V200) ? 4*1024 : 2*1024;

  printl2(0, _("ROM dumping...\n"));

  // Copies ROM dump program into a file
  f = fopen(DUMP_ROM89_FILE, "wb");
  if (f == NULL)
    return ERR_FILE_OPEN;

  fwrite(romDump89, sizeof(unsigned char), romDumpSize89, f);
  fclose(f);

  // Transfer program to calc
  TRYF(ti89_send_var(DUMP_ROM89_FILE, MODE_SEND_ONE_VAR, NULL));
  unlink(DUMP_ROM89_FILE);

  // Launch calculator program by remote control
  sprintf(update->label_text, _("Launching..."));
  update_label();

  TRY(ti89_send_key(KEY89_CLEAR));
  PAUSE(50);
  TRY(ti89_send_key(KEY89_CLEAR));
  PAUSE(50);
  TRY(ti89_send_key('m'));
  TRY(ti89_send_key('a'));
  TRY(ti89_send_key('i'));
  TRY(ti89_send_key('n'));
  TRY(ti89_send_key('\\'));
  TRY(ti89_send_key('r'));
  TRY(ti89_send_key('o'));
  TRY(ti89_send_key('m'));
  TRY(ti89_send_key('d'));
  TRY(ti89_send_key('u'));
  TRY(ti89_send_key('m'));
  TRY(ti89_send_key('p'));
  TRY(ti89_send_key(KEY89_LP));
  TRY(ti89_send_key(KEY89_RP));
  TRY(ti89_send_key(KEY89_ENTER));

  // Open file
  file = fopen(filename, "wb");
  if (file == NULL)
    return ERR_OPEN_FILE;

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  // Receive it now blocks per blocks (1024 + CHK)
  update_start();
  sprintf(update->label_text, _("Receiving..."));
  update_label();

  start = time(NULL);

  for (i = 0; i < ROMSIZE; i++) {
    sum = 0;
    update->total = 1024;

    for (j = 0; j < 1024; j++) {
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      sum += data;

      update->count = j;
      update_pbar();
      if (update->cancel)
	return -1;
    }

    TRY(cable->get(&data));
    checksum = data << 8;
    TRY(cable->get(&data));
    checksum |= data;
    if (sum != checksum)
      return ERR_CHECKSUM;
    TRY(cable->put(0xda));

    update->main_percentage = (float) i / (ROMSIZE);
    if (update->cancel)
      return -1;

    elapsed = (long) difftime(time(NULL), start);
    estimated = (long) (elapsed * (float) (ROMSIZE) / i);
    remaining = (long) difftime(estimated, elapsed);
    sprintf(buffer, "%s", ctime(&remaining));
    sscanf(buffer, "%3s %3s %i %s %i", tmp, tmp, &pad, tmp, &pad);
    sprintf(update->label_text, _("Remaining (mm:ss): %s"), tmp + 3);
    update_label();
  }

  TRY(cable->put(0xcc));	// make ROM dumping program exit.
  fclose(file);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti89_get_idlist(char *id)
{
  uint32_t varsize;
  uint8_t vartype;
  uint8_t varname[9];

  printl2(0, _("Getting ID list...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Getting variable..."));
  update_label();

  TRYF(ti89_send_REQ(0x0000, TI89_IDLIST, ""));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_VAR(&varsize, &vartype, (char*)varname));
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_CTS());
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_XDP(&varsize, (uint8_t*)id));
  id[varsize] = '\0';
  TRYF(ti89_send_ACK());

  TRYF(ti89_recv_EOT());
  TRYF(ti89_send_ACK());

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti89_get_clock(TicalcClock * clock, int mode)
{
  uint32_t varsize;
  uint8_t vartype;
  uint8_t varname[9];
  uint8_t buffer[32];

  printl2(0, _("Getting clock...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Getting clock..."));
  update_label();

  TRYF(ti89_send_REQ(0x0000, TI89_CLK, "Clock"));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_VAR(&varsize, &vartype, (char*)varname));
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_CTS());
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_XDP(&varsize, buffer));
  TRYF(ti89_send_ACK());

  TRYF(ti89_recv_EOT());
  TRYF(ti89_send_ACK());

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  clock->year = (buffer[6] << 8) | buffer[7];
  clock->month = buffer[8];
  clock->day = buffer[9];
  clock->hours = buffer[10];
  clock->minutes = buffer[11];
  clock->seconds = buffer[12];
  clock->date_format = buffer[13];
  clock->time_format = buffer[14];

  return 0;
}

int ti89_set_clock(const TicalcClock * clock, int mode)
{
  uint8_t buffer[16] = { 0 };
  uint16_t status;

  buffer[6] = clock->year >> 8;
  buffer[7] = clock->year & 0x00ff;
  buffer[8] = clock->month;
  buffer[9] = clock->day;
  buffer[10] = clock->hours;
  buffer[11] = clock->minutes;
  buffer[12] = clock->seconds;
  buffer[13] = clock->date_format;
  buffer[14] = clock->time_format;
  buffer[15] = 0xff;

  printl2(0, _("Setting clock...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Setting clock..."));
  update_label();

  TRYF(ti89_send_RTS(0x10, TI89_CLK, "Clock"));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_recv_CTS());
  TRYF(ti89_send_ACK());

  TRYF(ti89_send_XDP(0x10, buffer));
  TRYF(ti89_recv_ACK(NULL));

  TRYF(ti89_send_EOT());
  TRYF(ti89_recv_ACK(&status));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}
