/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc_err.h"
#include "defs82.h"
#include "calc_ext.h"
#include "trans.h"
//#include "rom82.h" // to do...
#include "update.h"

/* Functions used by TI_PC functions */

/* The PC indicates that is OK */
/* 02 56 00 00 */
static int PC_replyOK_82(void)
{
  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

/* The PC indicates that it is ready or wait data */
/* 02 09 00 00 */
static int PC_waitdata_82(void)
{
  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");

  return 0;
}

/* Check whether the TI reply OK */
/* 82 56 00 00 */
static int ti82_isOK(void)
{
  byte data;
 
  TRY(cable->get(&data));
  if(data != TI82_PC)
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != CMD82_TI_OK)
    {
      if(data == CMD82_CHK_ERROR) 
	return ERR_CHECKSUM;
      else 
	return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    {
      //printf("Debug: %02X\n", data);
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    {
      return ERR_NOT_READY;
    }
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

/* Check whether the TI reply OK with packet length */
/* 82 56 LL HH */
static int ti82_isPacketOK(word length)
{
  byte data;
  word w;

  TRY(cable->get(&data));
  if(data != TI82_PC) return ERR_NOT_REPLY;
  TRY(cable->get(&data));
  if(data != CMD82_TI_OK)
    {
      if(data==CMD82_CHK_ERROR) return ERR_CHECKSUM;
      else return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  w=data;
  TRY(cable->get(&data));
  w|=(data << 8);
  if(w != length) return ERR_PACKET;
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

/* The TI indicates that it is ready or wait data */
/* 82 09 00 00 */
static int ti82_waitdata(byte *rej_code)
{
  byte data;
  *rej_code = CMD82_REJ_NONE;

  TRY(cable->get(&data));
  if(data != TI82_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD82_WAIT_DATA) 
    {
      if(data != CMD82_REFUSED)
	{
	  DISPLAY("Data: %02X\n", data);
	  return ERR_INVALID_BYTE;
	}
      else
	{
	  DISPLAY("Var rejected... ");
	  TRY(cable->get(&data));
	  TRY(cable->get(&data));
	  TRY(cable->get(&data));
	  *rej_code = data;
	  //DISPLAY("Rejection code: %02X\n", data);
	  TRY(cable->get(&data));
	  TRY(cable->get(&data));
	  return ERR_VAR_REFUSED;
	}
    }
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  DISPLAY("The calculator wait data.\n");

  return 0;
}

// Check whether the TI reply that it is ready
int ti82_isready(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

// Send a string of characters to the TI
static int ti82_sendstring(char *s, word *checksum)
{

  int i;

  for(i=0; i<(int)strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (0xFF & s[i]); // The 0xFF is important else some variables can not be transmitted     
    }
  for(i=0; i<(int)(8-strlen(s)); i++)
    {
      TRY(cable->put(0x00));
    }

  return 0;
}

#define TI82_MAXTYPES 16
const char *TI82_TYPES[TI82_MAXTYPES]=
{ 
"REAL", "LIST", "MAT", "EQU", "??", "PRGM", "PPGM", "PIC", 
"GDB", "??", "??", "WDW", "ZSTO", "TAB", "LCD", "BACKUP"
};
const char *TI82_EXT[TI82_MAXTYPES]=
{
"82n", "82l", "82m", "82y", "82?", "82p", "82p", "82i",
"82d", "82?", "82?", "82w", "82z", "82t", "82?", "82b"
};

// Return the type corresponding to the value
const char *ti82_byte2type(byte data)
{
  if(data>TI82_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return "??";
    }
  else 
    {
      return TI82_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti82_type2byte(char *s)
{
  int i;

  for(i=0; i<TI82_MAXTYPES; i++)
    {
      if(!strcmp(TI82_TYPES[i], s)) break;
    }
  if(i>TI82_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti82_byte2fext(byte data)
{
  if(data>TI82_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");    
      return ".82?";
    }
  else 
  {
    return TI82_EXT[data];
  }
}

// Return the value corresponding to the file extension
byte ti82_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI82_MAXTYPES; i++)
    {
      if(!strcmp(TI82_EXT[i], s)) break;
    }
  if(i > TI82_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// General functions

int ti82_send_key(int key)
{
  return ERR_VOID_FUNCTION;
}

int ti82_remote_control(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

int ti82_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i;

  TRY(cable->open_port());
  update_start();
  sc->width=TI82_COLS;
  sc->height=TI82_ROWS;
  sc->clipped_width=TI82_COLS;
  sc->clipped_height=TI82_ROWS;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI82_COLS*TI82_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Request screendump.\n");
  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  
  TRY(ti82_isOK());
  TRY(cable->get(&data));
  if(data != TI82_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD82_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  max_cnt=data;
  TRY(cable->get(&data));
  max_cnt += (data << 8);
  DISPLAY("0x%04X = %i bytes to receive\n", max_cnt, max_cnt);
  DISPLAY("Screendump in progress...\n");

  update->total = max_cnt;
  for(i=0; i<max_cnt; i++)
    {
      TRY(cable->get(&data));
      (*bitmap)[i]=~data;
      sum+=data;
      
      update->count = i;
      update->percentage = (float)i/max_cnt;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(sum != checksum) return ERR_CHECKSUM;

  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_PC_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("PC reply OK.\n");
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti82_receive_backup(FILE *file, int mask_mode, longword *version)
{
  byte data;
  word sum;
  word checksum;
  word file_checksum;
  word block_size, size;
  int i, j;
  char desc[43]="Backup file received by TiLP";
  long offset;

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Waiting backup...");
  update_label();
  file_checksum=0;
  fprintf(file, "**TI82**");
  fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
  for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
  offset=ftell(file);
  fprintf(file, "XX");
  size=0;
  sum=0;
  DISPLAY("Receiving backup...\n");
  TRY(cable->get(&data)); 
  if(data != TI82_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD82_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  j=data; /* j should be equal to 0x09 but to be the safe side ... */
  if(data != 0x09) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  j|=(data << 8);
  if(data != 0x00) return ERR_INVALID_BYTE;
  DISPLAY("Receiving header.\n");
  sprintf(update->label_text, "Receiving...");
  update_label();
  for(i=0; i<j; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
      file_checksum+=data;
      fprintf(file, "%c", data); 
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_82());
  TRY(PC_waitdata_82());
  TRY(ti82_isOK());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      DISPLAY("Receiving part %i\n", i+1);
      TRY(cable->get(&data));
      if(data != TI82_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD82_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      block_size=data;
      fprintf(file, "%c", data);
      file_checksum+=data;
      TRY(cable->get(&data));
      block_size+=(data << 8);
      fprintf(file, "%c", data);
      file_checksum+=data;
      j=0;
      sum=0;
      size+=block_size;
      update->total = block_size;
      for(j=0; j<block_size; j++)
	{
	  TRY(cable->get(&data));
          sum+=data;
	  file_checksum+=data;
          fprintf(file, "%c", data);

	  update->count = j;
	  update->percentage = (float)j/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(sum != checksum) return ERR_CHECKSUM;

      TRY(PC_replyOK_82());
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
  DISPLAY("Backup complete.\n");
  fseek(file, offset, SEEK_SET);
  size+=2;
  fprintf(file, "%c%c", LSB(size+15), MSB(size+15));
  fseek(file, 0L, SEEK_END);
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti82_send_backup(FILE *file, int mask_mode)
{
  int err = 0;
  byte data;
  char str[128];
  word sum;
  int i;
  int j;
  word block_size;
  byte rej_code = CMD82_REJ_NONE;

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Sending...");
  update_label();
  DISPLAY("Sending backup...\n");
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI82**") && strcmp(str, "**TI82**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  fprintf(stderr, "MODE_FILE_CHK_ALL\n");
	  if( strcmp(str, "**TI82**"))
	    {
	      return ERR_INVALID_TI82_FILE;
	    }
	}
    }
  for(i=0; i<3; i++) fgetc(file);
  for(i=0; i<42; i++)
  {
    fgetc(file);
  }
  data=fgetc(file);
  //printf("%02X\n", data);
  data=fgetc(file);
  //printf("%02X\n", data);
  sum=0;
  DISPLAY("Sending header.\n");
  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_VAR_HEADER));
  data=fgetc(file);
  TRY(cable->put(data));
  block_size=data;
  data=fgetc(file);
  TRY(cable->put(data));
  block_size+=data;
  for(i=0; i<9; i++)
    {
      data=fgetc(file);
      sum+=data;
      TRY(cable->put(data));
    }
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  DISPLAY("The calculator should ask you to continue the transfer.\n");
  
  TRY(ti82_isPacketOK(block_size));

  /* Here, the calc is waiting until the user has confirmed the operation */
  sprintf(update->label_text, "Waiting confirmation on calc...");
  update_label();
  do
    {
      update_refresh();
      if(update->cancel) return ERR_ABORT;
      err=ti82_waitdata(&rej_code); 
      if(err == ERR_VAR_REFUSED)
	{
	  //fprintf(stderr, "Rejection code: 0x%02x\n", rej_code);
	  switch(rej_code)
	    {
	    case CMD82_REJ_SKIP:
	      fprintf(stderr, "Variable skipped by user\n");
	      //for(i=0; i<varsize; i++) fgetc(file); // read file anyway
	      TRY(PC_replyOK_82());
	      goto label_skip;
	      break;
	    case CMD82_REJ_EXIT:
	      fprintf(stderr, "Transfer cancelled by user\n");
	      goto label_exit;
	      break;
	    case CMD82_REJ_OUTOFMEM:
	      fprintf(stderr, "Out of mem\n");
	      return ERR_OUT_OF_MEMORY;
	      break;
	    default:
	      return ERR_INVALID_BYTE;
	      break;
	    }
	}
    }
  while( (err == 35) || (err == 3) );
  DISPLAY("The calculator continue the transfer.\n");

  sprintf(update->label_text, "Transmitting...");
  update_label();
			
  TRY(PC_replyOK_82());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
      DISPLAY("Sending part %i...\n", i+1);
      sum=0;
      TRY(cable->put(PC_TI82));
      TRY(cable->put(CMD82_DATA_PART));
      
      
      data=fgetc(file);
      block_size=data;
      TRY(cable->put(data));
      data=fgetc(file);
      block_size+=data << 8;
      TRY(cable->put(data));
      printf("Backup block size: %04X\n", block_size);
      update->total = block_size;
      for(j=0; j<block_size; j++)
        {
          data=fgetc(file);
          sum+=data;
          TRY(cable->put(data));

	  update->count = j;
	  update->percentage = (float)j/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti82_isPacketOK(block_size));
    }
  TRY(PC_replyOK_82());
  DISPLAY("\n");
 label_skip:
 label_exit:
  update_start();
  TRY(cable->close_port());
  
  return 0;
}

int ti82_directorylist(struct varinfo *list, int *n_elts)
{
  /* This function does not exist */
  update_start();

  return ERR_VOID_FUNCTION;
}

/* 
   Receive one or more variables: if varname[0]='\0' -> 
   group file else single file 
*/
int ti82_receive_var(FILE *file, int mask_mode, 
		     char *varname, byte vartype, byte varlock)
{
  byte data;
  word sum;
  word checksum, file_checksum;
  word var_size;
  char name[9];
  byte var_type;
  int i;
  int nvars;
  word w;
  long offset;
  char trans[9];
  char desc[43]="File received by TiLP";
  word allvars_size;	// This limits the size of a TIGL file to 64 Kb */
  int k;

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Waiting var(s)...");
  update_label();
  file_checksum=0;
  allvars_size=0;
  DISPLAY("Receiving variable(s)...\n");
  var_size=0;
  fprintf(file, "**TI82**");
  fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
  for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
  offset=ftell(file);
  fprintf(file, "XX");
  for(nvars=0; ;nvars++)
    {
      sum=0;
      TRY(cable->get(&data));
      if(data != TI82_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data == CMD82_EOT) break;
      else if(data != CMD82_VAR_HEADER) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != 0x0B) return ERR_INVALID_BYTE;
      fprintf(file, "%c", data);
      file_checksum+=data;
      TRY(cable->get(&data));
      if(data != 0x00) return ERR_INVALID_BYTE;
      fprintf(file, "%c", data);
      file_checksum+=data;
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      var_size=data;
      sum+=data;
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      var_size |= (data << 8);
      sum+=data;
      if(allvars_size+var_size+15 < allvars_size) 
	return ERR_GRP_SIZE_EXCEEDED;
      allvars_size+=var_size+15;
      DISPLAY("-> Size: 0x%04X = %i.\n", var_size, var_size);
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      var_type=data;
      DISPLAY("-> Type: %s\n", ti82_byte2type(data));
      sum+=data;
      for(i=0; i<8; i++)
	{
	  TRY(cable->get(&data));
	  fprintf(file, "%c", 0xFF & data);
	  sum+=data;
	  name[i]=data;
	}
      name[i]='\0';
      strcpy(varname, name);
      DISPLAY("-> Name: %s <", varname);
      for(k=0; k<8; k++) DISPLAY("%02X.", 0xFF & varname[k]);
      DISPLAY(">\n");
      DISPLAY("-> Translated name: %s\n", ti82_translate_varname(varname, trans, var_type));
      file_checksum += sum;
      sprintf(update->label_text, "Variable: %s", 
	      ti82_translate_varname(varname, trans, var_type));
      update_label();
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;
      
      TRY(PC_replyOK_82());
      TRY(PC_waitdata_82());
      DISPLAY("The calculator want continue.\n");
      
      TRY(ti82_isOK());
      DISPLAY("Receiving variable...\n");
      TRY(cable->get(&data));
      if(data != TI82_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD82_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      file_checksum+=data;
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      file_checksum+=data;
      sum=0;
      update->total = var_size;
      for(i=0; i<var_size; i++)
	{
	  TRY(cable->get(&data));
	  fprintf(file, "%c", data);
	  sum+=data;
	  
	  update->count = i;
	  update->percentage = (float)i/var_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      file_checksum+=sum;
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;
      
      TRY(PC_replyOK_82());
    }
  DISPLAY("The calculator does not want continue.\n");
  TRY(cable->get(&data));
  w=data;
  TRY(cable->get(&data));
  w |= (data << 8);
  if(w != var_size) return ERR_PACKET;
  TRY(PC_replyOK_82());
  fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
  fseek(file, offset, SEEK_SET);
  if(nvars == 1)
    fprintf(file, "%c%c", LSB(var_size+15), MSB(var_size+15));
  else
    fprintf(file, "%c%c", LSB(allvars_size), MSB(allvars_size));
  fseek(file, 0L, SEEK_END);
  if(nvars > 1) varname[0]='\0';
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti82_send_var(FILE *file, int mask_mode)
{
  int err = 0;
  byte data;
  word sum;
  word block_size;
  longword varsize;
  char varname[9];
  byte vartype;
  int i;
  char trans[9];
  byte rej_code = CMD82_REJ_NONE;
  char str[9];
 
  TRY(cable->open_port());
  update_start();
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI82**") && strcmp(str, "**TI82**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  fprintf(stderr, "MODE_FILE_CHK_ALL\n");
	  if( strcmp(str, "**TI82**"))
	    {
	      return ERR_INVALID_TI82_FILE;
	    }
	}
    }
  for(i=0; i<3; i++) fgetc(file);
  for(i=0; i<42; i++)
  {
    fgetc(file);
  }
  data=fgetc(file);
  data=fgetc(file);
  while(!feof(file))
    {
      data=fgetc(file);
      data=fgetc(file);
      if(feof(file)) break;
      varsize=fgetc(file);
      if(feof(file)) break;
      varsize+=fgetc(file) << 8;
      vartype=fgetc(file);
      for(i=0; i<8; i++) varname[i]=fgetc(file);
      varname[i]='\0';
      fgetc(file);
      fgetc(file);
      DISPLAY("Sending variable...\n");
      DISPLAY("-> Name: %s\n", varname);
      DISPLAY("-> Translated name: %s\n", 
	      ti82_translate_varname(varname, trans, vartype));
      DISPLAY("-> Size: %08X\n", varsize);
      DISPLAY("-> Type: %s\n", ti82_byte2type(vartype));
      sprintf(update->label_text, "Variable: %s", 
	      ti82_translate_varname(varname, trans, vartype));
      update_label();
      sum=0;
      TRY(cable->put(PC_TI82));
      TRY(cable->put(CMD82_VAR_HEADER));
      block_size=0x0B;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      data=LSB(varsize);
      sum+=data;
      TRY(cable->put(data));
      data=MSB(varsize);
      sum+=data;
      TRY(cable->put(data));
      data=vartype;
      sum+=data;
      TRY(cable->put(data));
      TRY(ti82_sendstring(varname, &sum));
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti82_isPacketOK(block_size));
      
      /* Here, the calc wait an action of the user: skip, overwrite, rename
	 or quit */
      sprintf(update->label_text, "Waiting confirmation on calc...");
      update_label();
      do
	{
	  update_refresh();
	  if(update->cancel) return ERR_ABORT;
	  err=ti82_waitdata(&rej_code);
	  if(err == ERR_VAR_REFUSED)
	    {
	      switch(rej_code)
		{
		case CMD82_REJ_SKIP:
		  fprintf(stderr, "Variable skipped by user\n");
		  for(i=0; i<(int)varsize; i++) fgetc(file); // read file anyway
		  TRY(PC_replyOK_82());
		  goto label_skip;
		  break;
		case CMD82_REJ_EXIT:
		  fprintf(stderr, "Transfer cancelled by user\n");
		  goto label_exit;
		  break;
		case CMD82_REJ_OUTOFMEM:
		  fprintf(stderr, "Out of mem\n");
		  return ERR_OUT_OF_MEMORY;
                  break;
		default:
		  return ERR_INVALID_BYTE;
		  break;
		}
	    }
	}
      while( (err == 35) || (err == 3) );

      sprintf(update->label_text, "Variable: %s",
              ti82_translate_varname(varname, trans, vartype));
      update_label();
      
      sum=0;
      TRY(PC_replyOK_82());
      TRY(cable->put(PC_TI82));
      TRY(cable->put(CMD82_DATA_PART));
      block_size=(unsigned short)varsize;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      update->total = block_size;
      for(i=0; i<block_size; i++)
	{
	  data=fgetc(file);
	  TRY(cable->put(data));
	  sum+=data;
	  
	  update->count = i;
	  update->percentage = (float)i/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));

      TRY(ti82_isPacketOK(block_size));
    label_skip:
	  while(0);
    }
  if( (mask_mode & MODE_SEND_ONE_VAR) ||
      (mask_mode & MODE_SEND_LAST_VAR) )
    {
      // The last var
      TRY(cable->put(PC_TI82));
      TRY(cable->put(CMD82_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
      DISPLAY("The computer does not want continue.\n");
      TRY(ti82_isOK());
    }  
  DISPLAY("\n");
 label_exit:
  update_start();
  TRY(cable->close_port());

  return 0;
}

#define DUMP_ROM82_FILE "dumprom.82p"

int ti82_dump_rom(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti82_get_rom_version(char *version)
{
  return ERR_VOID_FUNCTION;
}

int ti82_send_flash(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}
