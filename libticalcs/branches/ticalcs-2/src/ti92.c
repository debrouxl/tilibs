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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  This unit provides TI92 support.
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
#include "cmd92.h"
#include "rom92f2.h"
#include "keys92p.h"
#include "pause.h"
#include "dirlist.h"
#include "printl.h"

// Screen coordinates of the TI92
#define TI92_ROWS  128
#define TI92_COLS  240

int ti92_supported_operations(void)
{
  return
      (OPS_ISREADY |
       OPS_SCREENDUMP |
       OPS_SEND_KEY | OPS_RECV_KEY | OPS_REMOTE |
       OPS_DIRLIST |
       OPS_SEND_BACKUP | OPS_RECV_BACKUP |
       OPS_SEND_VARS | OPS_RECV_VARS | OPS_ROMVERSION | OPS_ROMDUMP);
}

int ti92_send_key(uint16_t key)
{
  LOCK_TRANSFER();
  TRYF(cable->open());

  TRYF(ti92_send_KEY(key));
  TRYF(ti92_recv_ACK(NULL));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_isready(void)
{
  uint16_t status;

  printl2(0, _("Is calculator ready ?\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  TRYF(ti92_send_RDY());
  TRYF(ti92_recv_ACK(&status));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return (status & 0x01) ? ERR_NOT_READY : 0;
}

int ti92_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc)
{
  uint32_t max_cnt;
  int err;

  printl2(0, _("Receiving screendump...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sc->width = TI92_COLS;
  sc->height = TI92_ROWS;
  sc->clipped_width = TI92_COLS;
  sc->clipped_height = TI92_ROWS;

  if (*bitmap != NULL)
    free(*bitmap);
  (*bitmap) =
      (uint8_t *) malloc(TI92_COLS * TI92_ROWS * sizeof(uint8_t) / 8);
  if ((*bitmap) == NULL) {
    printl2(2, "Unable to allocate memory.\n");
    exit(0);
  }

  TRYF(ti92_send_SCR());
  TRYF(ti92_recv_ACK(NULL));

  err = ti92_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
  if (err != ERR_CHECKSUM) {
  TRYF(err)};
  TRYF(ti92_send_ACK());

  printl2(0, _("Done.\n"));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_directorylist(TNode ** tree, uint32_t * memory)
{
  uint32_t unused;
  uint8_t buffer[65536];
  int err;
  TiVarEntry info;
  char folder_name[9] = "";
  TNode *vars, *apps;
  TNode *folder = NULL;

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  printl2(0, _("Directory listing...\n"));

  TRYF(ti92_send_REQ(0, TI92_RDIR, ""));
  TRYF(ti92_recv_ACK(NULL));

  TRYF(ti92_recv_VAR(&info.size, &info.type, info.name));

  *tree = t_node_new(NULL);
  vars = t_node_new(NULL);
  apps = t_node_new(NULL);
  t_node_append(*tree, vars);
  t_node_append(*tree, apps);

  for (;;) {
    TiVarEntry *ve = calloc(1, sizeof(TiVarEntry));
    TNode *node;

    TRYF(ti92_send_ACK());
    TRYF(ti92_send_CTS());

    TRYF(ti92_recv_ACK(NULL));
    TRYF(ti92_recv_XDP(&unused, buffer));
    memcpy(ve->name, buffer + 4, 8);	// skip 4 extra 00 uint8_t
    ve->name[8] = '\0';
    ve->type = buffer[12];
    ve->attr = buffer[13];
    ve->size = buffer[14] | (buffer[15] << 8) |
	(buffer[16] << 16) | (buffer[17] << 24);
    strcpy(ve->folder, "");

    tifiles_translate_varname(ve->name, ve->trans, ve->type);
    node = t_node_new(ve);

    if (ve->type == TI92_DIR) {
      strcpy(folder_name, ve->name);
      folder = t_node_append(vars, node);
    } else {
      strcpy(ve->folder, folder_name);
      t_node_append(folder, node);
    }

    printl2(0, _("Name: %8s | "), ve->name);
    printl2(0, _("Type: %8s | "), tifiles_vartype2string(ve->type));
    printl2(0, _("Attr: %i  | "), ve->attr);
    printl2(0, _("Size: %08X\n"), ve->size);

    TRYF(ti92_send_ACK());
    err = ti92_recv_CONT();
    if (err == ERR_EOT)
      break;
    TRYF(err);

    sprintf(update->label_text, _("Reading of '%s/%s'"),
	    ((TiVarEntry *) (folder->data))->trans, ve->trans);
    update_label();
    if (update->cancel)
      return -1;
  }

  TRYF(ti92_send_ACK());

  *memory = ticalc_dirlist_memused(*tree);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_recv_backup(const char *filename, int mask_mode)
{
  Ti9xBackup content = { 0 };
  uint32_t block_size;
  int block, err;
  uint32_t unused;
  uint16_t unused2;
  uint8_t *ptr;

  printl2(0, _("Receiving backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  content.calc_type = CALC_TI92;
  sprintf(update->label_text, _("Receiving backup..."));
  update_label();

  // silent request
  TRYF(ti92_send_REQ(0, TI92_BKUP, "main\\backup"));
  TRYF(ti92_recv_ACK(&unused2));

  content.data_part = (uint8_t *) tifiles_calloc(128 * 1024, 1);
  content.type = TI92_BKUP;
  content.data_length = 0;

  for (block = 0;; block++) {
    sprintf(update->label_text, _("Receiving block %2i"), block);
    update_label();
    err = ti92_recv_VAR(&block_size, &content.type, content.rom_version);
    TRYF(ti92_send_ACK());

    if (err == ERR_EOT)
      break;
    TRYF(err);

    TRYF(ti92_send_CTS());
    TRYF(ti92_recv_ACK(NULL));

    ptr = content.data_part + content.data_length;
    TRYF(ti92_recv_XDP(&unused, ptr));
    memmove(ptr, ptr + 4, block_size);
    TRYF(ti92_send_ACK());
    content.data_length += block_size;
  }

  strcpy(content.comment, "Backup file received by TiLP");

  ti9x_write_backup_file(filename, &content);
  ti9x_free_backup_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_send_backup(const char *filename, int mask_mode)
{
  Ti9xBackup content = { 0 };
  int i, nblocks;

  printl2(0, _("Sending backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending backup..."));
  update_label();

  TRYF(ti9x_read_backup_file(filename, &content));

  TRYF(ti92_send_VAR(content.data_length, TI92_BKUP, content.rom_version));
  TRYF(ti92_recv_ACK(NULL));

  nblocks = content.data_length / 1024;
  for (i = 0; i <= nblocks; i++) {
    uint32_t length = (i != nblocks) ? 1024 : content.data_length % 1024;

    TRYF(ti92_send_VAR(length, TI92_BKUP, content.rom_version));
    TRYF(ti92_recv_ACK(NULL));

    TRYF(ti92_recv_CTS());
    TRYF(ti92_send_ACK());

    TRYF(ti92_send_XDP(length, content.data_part + 1024 * i));
    TRYF(ti92_recv_ACK(NULL));

    ((update->main_percentage)) = (float) i / nblocks;
    update_pbar();
    if (update->cancel)
      return -1;
  }

  TRYF(ti92_send_EOT());

  ti9x_free_backup_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_recv_var(char *filename, int mask_mode, TiVarEntry * entry)
{
  static Ti9xRegular *content;
  uint16_t status;
  TiVarEntry *ve;
  char *fn;
  static int nvar = 0;
  uint32_t unused;
  uint8_t varname[18], utf8[35];

  printl2(0, _("Receiving variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  if ((mask_mode & MODE_RECEIVE_FIRST_VAR) ||
      (mask_mode & MODE_RECEIVE_SINGLE_VAR)) {
    content = ti9x_create_regular_content();
    nvar = 0;
  }

  content->calc_type = CALC_TI92;
  content->entries = (TiVarEntry *) tifiles_realloc(content->entries,
						    (nvar +
						     1) *
						    sizeof(TiVarEntry));
  ve = &(content->entries[nvar]);
  memcpy(ve, entry, sizeof(TiVarEntry));

  strcpy(varname, entry->folder);
  strcat(varname, "\\");
  strcat(varname, entry->name);

  tifiles_translate_varname(varname, utf8, entry->type);
  sprintf(update->label_text, _("Receiving '%s'"), utf8);
  update_label();

  TRYF(ti92_send_REQ(0, entry->type, varname));
  TRYF(ti92_recv_ACK(&status));
  if (status != 0)
    return ERR_MISSING_VAR;

  TRYF(ti92_recv_VAR(&ve->size, &ve->type, ve->name));
  TRYF(ti92_send_ACK());

  TRYF(ti92_send_CTS());
  TRYF(ti92_recv_ACK(NULL));

  ve->data = tifiles_calloc(ve->size + 4, 1);
  TRYF(ti92_recv_XDP(&unused, ve->data));
  memmove(ve->data, ve->data + 4, ve->size);
  TRYF(ti92_send_ACK());

  TRYF(ti92_recv_EOT());
  TRYF(ti92_send_ACK());

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

int ti92_send_var(const char *filename, int mask_mode, char **actions)
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
    uint8_t full_name[18], varname[18], utf8[35];

    if (actions == NULL)	// backup or old behaviour
      strcpy(varname, entry->name);
    else if (actions[i][0] == ACT_SKIP) {
      printl2(0, _(" '%s' has been skipped !\n"), entry->name);
      continue;
    } else if (actions[i][0] == ACT_OVER)
      strcpy(varname, actions[i] + 1);

    if (mask_mode & MODE_LOCAL_PATH)
      strcpy(full_name, varname);
    else {
      strcpy(full_name, entry->folder);
      strcat(full_name, "\\");
      strcat(full_name, varname);
    }

    tifiles_translate_varname(full_name, utf8, entry->type);
    sprintf(update->label_text, _("Sending '%s'"), utf8);
    update_label();

    TRYF(ti92_send_VAR(entry->size, entry->type, varname));
    TRYF(ti92_recv_ACK(NULL));

    TRYF(ti92_recv_CTS());
    TRYF(ti92_send_ACK());

    memcpy(buffer + 4, entry->data, entry->size);
    TRYF(ti92_send_XDP(entry->size + 4, buffer));
    TRYF(ti92_recv_ACK(&status));

    TRYF(ti92_send_EOT());
    TRYF(ti92_recv_ACK(NULL));

    printl2(0, "\n");
  }

  ti9x_free_regular_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti92_recv_var_2(char *filename, int mask_mode, TiVarEntry *entry)
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

		err = ti92_recv_VAR(&ve->size, &ve->type, tipath);
		TRYF(ti92_send_ACK());

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

		tifiles_translate_varname(ve->name, utf8, ve->type);
		sprintf(update->label_text, _("Receiving '%s'"), utf8);
		update_label();

		TRYF(ti92_send_CTS());
		TRYF(ti92_recv_ACK(NULL));

		ve->data = tifiles_calloc(ve->size + 4, 1);
		TRYF(ti92_recv_XDP(&unused, ve->data));
		memmove(ve->data, ve->data + 4, ve->size);
		TRYF(ti92_send_ACK());
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

int ti92_send_flash(const char *filename, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti92_recv_flash(const char *filename, int mask_mode, TiVarEntry * ve)
{
  return ERR_VOID_FUNCTION;
}

#define DUMP_ROM92_FILE "dumprom.92p"
#define ROMSIZE (1024*1024)

int ti92_dump_rom(const char *filename, int mask_mode)
{
  int i, j, k;
  uint8_t data;
  time_t start, elapsed, estimated, remaining;
  char buffer[257];
  char tmp[257];
  int pad;
  FILE *f, *file;
  uint16_t checksum, sum;

  printl2(0, _("ROM dumping...\n"));

  // Copies ROM dump program into a file
  f = fopen(DUMP_ROM92_FILE, "wb");
  if (f == NULL)
    return ERR_FILE_OPEN;

  fwrite(romDump92f2, sizeof(unsigned char), romDumpSize92f2, f);

  fclose(f);

  // Transfer program to calc
  TRYF(ti92_send_var(DUMP_ROM92_FILE, MODE_SEND_ONE_VAR, NULL));
  unlink(DUMP_ROM92_FILE);

  // Launch calculator program by remote control
  sprintf(update->label_text, _("Launching..."));
  update_label();

  TRY(ti92_send_key(KEY92P_CLEAR));
  PAUSE(50);
  TRY(ti92_send_key(KEY92P_CLEAR));
  PAUSE(50);
  TRY(ti92_send_key('m'));
  TRY(ti92_send_key('a'));
  TRY(ti92_send_key('i'));
  TRY(ti92_send_key('n'));
  TRY(ti92_send_key('\\'));
  TRY(ti92_send_key('d'));
  TRY(ti92_send_key('u'));
  TRY(ti92_send_key('m'));
  TRY(ti92_send_key('p'));
  TRY(ti92_send_key('r'));
  TRY(ti92_send_key('o'));
  TRY(ti92_send_key('m'));
  TRY(ti92_send_key(KEY92P_LP));
  TRY(ti92_send_key(KEY92P_RP));
  TRY(ti92_send_key(KEY92P_ENTER));

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

  for (i = 0, k = 0; i < mask_mode * 1024; i++) {
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

    update->count = 1024 * mask_mode;
    update->main_percentage = (float) i / (1024 * mask_mode);
    if (update->cancel)
      return -1;

    elapsed = (long) difftime(time(NULL), start);
    estimated = (long) (elapsed * (float) (1024 * mask_mode) / i);
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

int ti92_get_idlist(char *id)
{
  return ERR_VOID_FUNCTION;
}
