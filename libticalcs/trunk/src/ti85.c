/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2003  Romain Lievin
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
  This unit provides TI85 support.
  Note: the source code is the SAME as the TI82 support (same indentation).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "intl2.h"
#include "headers.h"
#include "externs.h"
#include "update.h"
#include "packets.h"
#include "calc_err.h"
#include "cmd85.h"
#include "rom85u.h"
#include "rom85z.h"

// Screen coordinates of the TI82
#define TI85_ROWS  64
#define TI85_COLS  128

int ti85_supported_operations(void)
{
  return
      (OPS_SCREENDUMP |
       OPS_SEND_BACKUP | OPS_RECV_BACKUP |
       OPS_SEND_VARS | OPS_RECV_VARS | OPS_ROMDUMP);
}

int ti85_isready(void)
{
  return ERR_VOID_FUNCTION;
}

int ti85_send_key(uint16_t key)
{
  return ERR_VOID_FUNCTION;
}

int ti85_directorylist(TNode ** tree, uint32_t * memory)
{
  return ERR_VOID_FUNCTION;
}

int ti85_screendump(uint8_t ** bitmap, int mask_mode,
		    TicalcScreenCoord * sc)
{
  uint16_t max_cnt;
  int err;

  DISPLAY(_("Receiving screendump...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sc->width = TI85_COLS;
  sc->height = TI85_ROWS;
  sc->clipped_width = TI85_COLS;
  sc->clipped_height = TI85_ROWS;

  if (*bitmap != NULL)
    free(*bitmap);
  (*bitmap) =
      (uint8_t *) malloc(TI85_COLS * TI85_ROWS * sizeof(uint8_t) / 8);
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


int ti85_recv_backup(const char *filename, int mask_mode)
{
  Ti8xBackup *content;
  uint8_t varname[9] = { 0 };

  DISPLAY(_("Receiving backup...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  content = ti8x_create_backup_content();
  content->calc_type = CALC_TI85;
  sprintf(update->label_text, _("Waiting for backup..."));
  update_label();

  TRYF(ti85_recv_VAR(&(content->data_length1), &content->type, varname));
  content->data_length2 = varname[0] | (varname[1] << 8);
  content->data_length3 = varname[2] | (varname[3] << 8);
  content->mem_address = varname[4] | (varname[5] << 8);
  TRYF(ti85_send_ACK());

  TRYF(ti85_send_CTS());
  TRYF(ti85_recv_ACK(NULL));

  content->data_part1 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length1, content->data_part1));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 1 / 3;
  content->data_part2 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length2, content->data_part2));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 2 / 3;
  content->data_part3 = tifiles_calloc(65536, 1);
  TRYF(ti85_recv_XDP(&content->data_length3, content->data_part3));
  TRYF(ti85_send_ACK());
  update->main_percentage = (float) 3 / 3;
  content->data_part4 = NULL;

  strcpy(content->comment, "Backup file received by TiLP");
  ti8x_write_backup_file(filename, content);
  ti8x_free_backup_content(content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti85_send_backup(const char *filename, int mask_mode)
{
  Ti8xBackup content = { 0 };
  int err = 0;
  uint16_t length;
  uint8_t varname[9];
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
  varname[4] = LSB(content.mem_address);
  varname[5] = MSB(content.mem_address);
  TRYF(ti85_send_VAR(content.data_length1, TI85_BKUP, varname));
  TRYF(ti85_recv_ACK(&status));

  sprintf(update->label_text, _("Waiting user's action..."));
  update_label();
  do {				// wait user's action
    update_refresh();
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
  update->main_percentage = (float) 1 / 3;
  TRYF(ti85_send_XDP(content.data_length2, content.data_part2));
  TRYF(ti85_recv_ACK(&status));
  update->main_percentage = (float) 2 / 3;
  TRYF(ti85_send_XDP(content.data_length3, content.data_part3));
  TRYF(ti85_recv_ACK(&status));
  update->main_percentage = (float) 3 / 3;

  ti8x_free_backup_content(&content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti85_recv_var(char *filename, int mask_mode, TiVarEntry * unused)
{
  int nvar = 0;
  Ti8xRegular *content;
  int err = 0;
  char *fn = NULL;

  DISPLAY(_("Receiving variable(s)...\n"));

  LOCK_TRANSFER();
  TRYF(cable->open());
  update_start();

  sprintf(update->label_text, _("Waiting var(s)..."));
  update_label();

  content = ti8x_create_regular_content();
  content->calc_type = CALC_TI85;

  for (nvar = 0;; nvar++) {
    TiVarEntry *ve;

    content->entries =
	(TiVarEntry *) tifiles_realloc(content->entries,
				       (nvar + 2) * sizeof(TiVarEntry));
    ve = &(content->entries[nvar]);

    do {
      update_refresh();
      if (update->cancel)
	return ERR_ABORT;
      err =
	  ti85_recv_VAR((uint16_t *) & (ve->size), &(ve->type), ve->name);
      fixup(ve->size);
    }
    while (err == ERR_READ_TIMEOUT);
    TRYF(ti85_send_ACK());
    if (err == ERR_EOT) {
      goto exit;
    }
    TRYF(err);

    TRYF(ti85_send_CTS());
    TRYF(ti85_recv_ACK(NULL));

    sprintf(update->label_text, _("Receiving '%s'"),
	    tifiles_translate_varname2(ve->name, ve->type));
    update_label();

    ve->data = tifiles_calloc(ve->size, 1);
    TRYF(ti85_recv_XDP((uint16_t *) & ve->size, ve->data));
    TRYF(ti85_send_ACK());
  }

exit:
  strcpy(content->comment, "Group file received by TiLP");
  content->num_entries = nvar;
  if (nvar > 1) {
    //strcpy(filename, "group.85g");
    ti8x_write_regular_file(filename, content, NULL);
  } else {
    ti8x_write_regular_file(NULL, content, &fn);
    strcpy(filename, fn);
    tifiles_free(fn);
  }
  ti8x_free_regular_content(content);

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti85_send_var(const char *filename, int mask_mode)
{
  Ti8xRegular content = { 0 };
  int i;
  int err;
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
    TiVarEntry *entry = &(content.entries[i]);;

    TRYF(ti85_send_VAR(entry->size, entry->type, entry->name));
    TRYF(ti85_recv_ACK(&status));

    sprintf(update->label_text, _("Waiting user's action..."));
    update_label();
    do {			// wait user's action
      update_refresh();
      if (update->cancel)
	return ERR_ABORT;
      err = ti85_recv_SKIP(&rej_code);
    }
    while (err == ERR_READ_TIMEOUT);
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

  if ((mask_mode & MODE_SEND_ONE_VAR) || (mask_mode & MODE_SEND_LAST_VAR)) {
    TRYF(ti85_send_EOT());
    TRYF(ti85_recv_ACK(NULL));
  }

  TRYF(cable->close());
  UNLOCK_TRANSFER();

  return 0;
}

int ti85_send_flash(const char *filename, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti85_recv_flash(const char *filename, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

#define DUMP_ROM85_FILE "dumprom.85p"
#define ROMSIZE 128

int ti85_dump_rom(const char *filename, int mask_mode)
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
  int b = 0;

  DISPLAY(_("ROM dumping...\n"));

  // Copies ROM dump program into a file
  f = fopen(DUMP_ROM85_FILE, "wb");
  if (f == NULL)
    return ERR_FILE_OPEN;

  if (mask_mode == SHELL_ZSHELL)
    fwrite(romDump85z, sizeof(unsigned char), romDumpSize85z, f);
  else
    fwrite(romDump85u, sizeof(unsigned char), romDumpSize85u, f);

  fclose(f);

  // Transfer program to calc
  TRYF(ti85_send_var(DUMP_ROM85_FILE, MODE_SEND_ONE_VAR));
  unlink(DUMP_ROM85_FILE);

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

int ti85_get_idlist(char *id)
{
  return ERR_VOID_FUNCTION;
}
