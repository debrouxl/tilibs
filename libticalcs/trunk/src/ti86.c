/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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
  This unit provides TI86 support
  Note: the source code is the SAME as the TI83 support (same indentation).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include "intl2.h"
#include "headers.h"
#include "externs.h"
#include "update.h"
#include "packets.h"
#include "calc_err.h"
#include "cmd85.h"
#include "rom86.h"
#include "pause.h"

// Screen coordinates of the TI86
#define TI86_ROWS  64
#define TI86_COLS  128

int ti86_supported_operations(void)
{
  return
      (OPS_SCREENDUMP |
       OPS_DIRLIST |
       OPS_SEND_BACKUP | OPS_RECV_BACKUP |
       OPS_SEND_VARS | OPS_RECV_VARS | OPS_ROMDUMP);
}

int ti86_isready(void)
{
  return ERR_VOID_FUNCTION;
}

int ti86_send_key(uint16_t key)
{
  LOCK_TRANSFER();
  TRYF(cable->open());

  TRYF(ti85_send_KEY(key));
  TRYF(ti85_recv_ACK(NULL));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti86_directorylist(TNode ** tree, uint32_t * memory)
{
  uint16_t unused;
  TNode *vars, *apps, *folder;
  uint8_t *mem = (uint8_t *) memory;
  uint8_t hl, ll, lh;

  DISPLAY(_("Directory listing...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  TRYF(ti85_send_REQ(0x0000, TI86_DIR, ""));
  TRYF(ti85_recv_ACK(&unused));

  TRYF(ti85_recv_XDP(&unused, (uint8_t *) memory));
  TRYF(ti85_send_ACK());

  hl = mem[0];
  ll = mem[1];
  lh = mem[2];
  *memory = (hl << 16) | (lh << 8) | ll;

  *tree = t_node_new(NULL);
  vars = t_node_new(NULL);
  apps = t_node_new(NULL);
  t_node_append(*tree, vars);
  t_node_append(*tree, apps);

  folder = t_node_new(NULL);
  t_node_append(vars, folder);

  for (;;) {
    TiVarEntry *ve = calloc(1, sizeof(TiVarEntry));
    TNode *node;
    int err;

    err = ti85_recv_VAR((uint16_t *) & ve->size, &ve->type, ve->name);
    fixup(ve->size);
    TRYF(ti85_send_ACK());
    if (err == ERR_EOT)
      break;
    else if (err != 0)
      return err;

    tifiles_translate_varname(ve->name, ve->trans, ve->type);
    node = t_node_new(ve);
    t_node_append(folder, node);

    sprintf(update->label_text, _("Reading of '%s'"), ve->trans);
    update_label();
    if (update->cancel)
      return ERR_ABORT;
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti86_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc)
{
  uint16_t max_cnt;
  int err;

  DISPLAY(_("Receiving screendump...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sc->width = TI86_COLS;
  sc->height = TI86_ROWS;
  sc->clipped_width = TI86_COLS;
  sc->clipped_height = TI86_ROWS;

  if (*bitmap != NULL)
    free(*bitmap);
  (*bitmap) =
      (uint8_t *) malloc(TI86_COLS * TI86_ROWS * sizeof(uint8_t) / 8);
  if ((*bitmap) == NULL) {
    fprintf(stderr, "Unable to allocate memory.\n");
    exit(0);
  }

  TRYF(ti85_send_SCR());
  TRYF(ti85_recv_ACK(NULL));

  err = ti85_recv_XDP(&max_cnt, *bitmap);	// pb with checksum
  if (err != ERR_CHECKSUM) {
  TRYF(err)};
  TRYF(ti85_send_ACK());

  DISPLAY(_("Done.\n"));

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

// same code as TI85, not TI83 (backup is not silent !)!
int ti86_recv_backup(const char *filename, int mask_mode)
{
  Ti8xBackup *content;
  uint8_t varname[9] = { 0 };

  DISPLAY(_("Receiving backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  content = ti8x_create_backup_content();
  content->calc_type = CALC_TI86;
  sprintf(update->label_text, _("Waiting backup..."));
  update_label();

  TRYF(ti85_recv_VAR(&(content->data_length1), &content->type, varname));
  content->data_length2 = varname[0] | (varname[1] << 8);
  content->data_length3 = varname[2] | (varname[3] << 8);
  content->data_length4 = varname[4] | (varname[5] << 8);
  TRYF(ti85_send_ACK());

  TRYF(ti85_send_CTS());
  TRYF(ti85_recv_ACK(NULL));

  content->data_part1 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length1, content->data_part1));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 1 / 4;
  content->data_part2 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length2, content->data_part2));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 2 / 4;
  if (content->data_length3) {
    content->data_part3 = tifiles_calloc(65536, 1);
    TRYF(ti85_recv_XDP(&content->data_length3, content->data_part3));
    TRYF(ti85_send_ACK());
  } else
    content->data_part3 = NULL;
  update->main_percentage = (float) 3 / 4;
  content->data_part4 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length4, content->data_part4));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 4 / 4;

  strcpy(content->comment, "Backup file received by TiLP");

  ti8x_write_backup_file(filename, content);
  ti8x_free_backup_content(content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

// same code as TI85, not TI83 (backup is not silent !)!
int ti86_send_backup(const char *filename, int mask_mode)
{
  Ti8xBackup content = { 0 };
  int err = 0;
  uint16_t length;
  uint8_t varname[9] = { 0 };
  uint8_t rej_code;
  uint16_t status;

  DISPLAY(_("Sending backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending..."));
  update_label();

  TRYF(ti8x_read_backup_file(filename, &content));

  length = content.data_length1;
  varname[0] = LSB(content.data_length2);
  varname[1] = MSB(content.data_length2);
  varname[2] = LSB(content.data_length3);
  varname[3] = MSB(content.data_length3);
  varname[4] = LSB(content.data_length4);
  varname[5] = MSB(content.data_length4);

  TRYF(ti85_send_VAR(content.data_length1, TI86_BKUP, varname));
  TRYF(ti85_recv_ACK(&status));

  sprintf(update->label_text, _("Waiting user's action..."));
  update_label();
  do {				// wait user's action
    if (update->cancel)
      return ERR_ABORT;
    err = ti85_recv_SKIP(&rej_code);
  }
  while (err == ERR_READ_TIMEOUT);
  TRYF(ti85_send_ACK());
  switch (rej_code) {
  case REJ_EXIT:
  case REJ_SKIP:
    return ERR_ABORT;
    break;
  case REJ_MEMORY:
    return ERR_OUT_OF_MEMORY;
    break;
  default:			// RTS
    break;
  }
  sprintf(update->label_text, _("Sending..."));
  update_label();

  TRYF(ti85_send_XDP(content.data_length1, content.data_part1));
  TRYF(ti85_recv_ACK(&status));
  update->main_percentage = (float) 1 / 4;
  TRYF(ti85_send_XDP(content.data_length2, content.data_part2));
  TRYF(ti85_recv_ACK(&status));
  update->main_percentage = (float) 2 / 4;
  if (content.data_length3) {
    TRYF(ti85_send_XDP(content.data_length3, content.data_part3));
    TRYF(ti85_recv_ACK(&status));
  }
  update->main_percentage = (float) 3 / 4;
  TRYF(ti85_send_XDP(content.data_length4, content.data_part4));
  TRYF(ti85_recv_ACK(&status));
  update->main_percentage = (float) 4 / 4;

  ti8x_free_backup_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti86_recv_var(char *filename, int mask_mode, TiVarEntry * entry)
{
  static Ti8xRegular *content;
  uint16_t unused;
  TiVarEntry *ve;
  char *fn;
  static int nvar = 0;

  DISPLAY(_("Receiving variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  if ((mask_mode & MODE_RECEIVE_FIRST_VAR) ||
      (mask_mode & MODE_RECEIVE_SINGLE_VAR)) {
    content = ti8x_create_regular_content();
    nvar = 0;
  }

  content->calc_type = CALC_TI86;
  content->entries = (TiVarEntry *) tifiles_realloc(content->entries,
						    (nvar +
						     1) *
						    sizeof(TiVarEntry));
  ve = &(content->entries[nvar]);
  memcpy(ve, entry, sizeof(TiVarEntry));

  sprintf(update->label_text, _("Receiving '%s'"),
	  tifiles_translate_varname2(entry->name, entry->type));
  update_label();

  // silent request
  TRYF(ti85_send_REQ(entry->size, entry->type, entry->name));
  TRYF(ti85_recv_ACK(&unused));

  TRYF(ti85_recv_VAR((uint16_t *) & ve->size, &ve->type, ve->name));
  TRYF(ti85_send_ACK());
  fixup(ve->size);

  TRYF(ti85_send_CTS());
  TRYF(ti85_recv_ACK(NULL));

  ve->data = tifiles_calloc(ve->size, 1);
  TRYF(ti85_recv_XDP((uint16_t *) & ve->size, ve->data));
  TRYF(ti85_send_ACK());

  if (++nvar > 1)
    strcpy(content->comment, "Group file received by TiLP");
  else
    strcpy(content->comment, "Single file received by TiLP");

  content->num_entries = nvar;
  if (mask_mode & MODE_RECEIVE_SINGLE_VAR) {	// single
    ti8x_write_regular_file(NULL, content, &fn);
    strcpy(filename, fn);
    tifiles_free(fn);
    ti8x_free_regular_content(content);
  } else if (mask_mode & MODE_RECEIVE_LAST_VAR) {	// group
    ti8x_write_regular_file(filename, content, NULL);
    ti8x_free_regular_content(content);
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  PAUSE(PAUSE_BETWEEN_VARS);

  return 0;
}

int ti86_send_var(const char *filename, int mask_mode, char **actions)
{
  Ti8xRegular content = { 0 };
  int i;
  uint8_t rej_code;
  uint16_t status;

  DISPLAY(_("Sending variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Sending..."));
  update_label();

  TRYF(ti8x_read_regular_file(filename, &content));

  for (i = 0; i < content.num_entries; i++) {
    TiVarEntry *entry = &(content.entries[i]);
    uint8_t varname[18];

    if (actions == NULL)	// backup or old behaviour
      strcpy(varname, entry->name);
    else if (actions[i][0] == ACT_SKIP) {
      DISPLAY(_(" '%s' has been skipped !\n"), entry->name);
      continue;
    } else if (actions[i][0] == ACT_OVER)
      strcpy(varname, actions[i] + 1);

    TRYF(ti85_send_RTS(entry->size, entry->type, varname));
    TRYF(ti85_recv_ACK(&status));

    TRYF(ti85_recv_SKIP(&rej_code));
    TRYF(ti85_send_ACK());

    switch (rej_code) {
    case REJ_EXIT:
      return ERR_ABORT;
      break;
    case REJ_SKIP:
      continue;
      break;
    case REJ_MEMORY:
      return ERR_OUT_OF_MEMORY;
      break;
    default:			// RTS
      break;
    }
    sprintf(update->label_text, _("Sending '%s'"),
	    tifiles_translate_varname2(entry->name, entry->type));
    update_label();

    TRYF(ti85_send_XDP(entry->size, entry->data));
    TRYF(ti85_recv_ACK(&status));

    DISPLAY("\n");
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  PAUSE(PAUSE_BETWEEN_VARS);

  return 0;
}

int ti86_send_flash(const char *filename, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti86_recv_flash(const char *filename, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

#define DUMP_ROM86_FILE "dumprom.86p"
#define ROMSIZE 256

int ti86_dump_rom(const char *filename, int mask_mode)
{
  int i, j;
  uint8_t data;
  time_t start, elapsed, estimated, remaining;
  char buffer[257];
  char tmp[257];
  int pad;
  FILE *f, *file;
  uint16_t checksum, sum;
  int err;
  int b = 0;;

  DISPLAY(_("ROM dumping...\n"));

  // Copies ROM dump program into a file
  f = fopen(DUMP_ROM86_FILE, "wb");
  if (f == NULL)
    return ERR_FILE_OPEN;

  fwrite(romDump86, sizeof(unsigned char), romDumpSize86, f);





  fclose(f);

  // Transfer program to calc
  TRYF(ti86_send_var(DUMP_ROM86_FILE, MODE_SEND_ONE_VAR, NULL));
  unlink(DUMP_ROM86_FILE);

  // Open file
  file = fopen(filename, "wb");
  if (file == NULL)
    return ERR_OPEN_FILE;

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  // Wait for user's action (execing program)
  sprintf(update->label_text, _("Waiting user's action..."));
  update_label();
  do {
    update_refresh();
    if (update->cancel)
      return ERR_ABORT;
    err = cable->get(&data);
    sum = data;
  }
  while (err == ERR_READ_TIMEOUT);
  fprintf(file, "%c", data);

  // Receive it now blocks per blocks (1024 + CHK)
  update_start();
  sprintf(update->label_text, _("Receiving..."));
  update_label();

  start = time(NULL);

  for (i = 0; i < ROMSIZE; i++) {
    if (b)
      sum = 0;
    update->total = 1024;

    for (j = 0; j < 1023 + b; j++) {
      TRYF(cable->get(&data));
      fprintf(file, "%c", data);
      sum += data;

      update->count = j;
      update_pbar();
      if (update->cancel)
	return -1;
    }
    b = 1;

    TRYF(cable->get(&data));
    checksum = data << 8;
    TRYF(cable->get(&data));
    checksum |= data;
    if (sum != checksum)
      return ERR_CHECKSUM;
    TRYF(cable->put(0xda));

    update->count = ROMSIZE;
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

  fclose(file);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti86_get_idlist(char *id)
{
  return ERR_VOID_FUNCTION;
}
