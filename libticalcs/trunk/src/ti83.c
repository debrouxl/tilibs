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
#include "defs83.h"
#include "calc_ext.h"
#include "trans.h"
#include "pause.h"
#include "rom83.h"
#include "update.h"

/* Functions used by TI_PC functions */

// The PC indicates that is OK
// 03 56 00 00
static int PC_replyOK_83(void)
{
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

// The PC indicates that it is ready or wait data
// 03 09 00 00
static int PC_waitdata_83(void)
{
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");
  
  return 0;
}

// Check whether the TI reply OK
// 83 56 00 00
static int ti83_isOK(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI83_PC) 
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != CMD83_TI_OK)
    { 
      if(data==CMD83_CHK_ERROR) 
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

// Check whether the TI reply OK with packet length
// 83 56 LL HH
static int ti83_isPacketOK(word length)
{
  byte data;
  word w;

  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83_TI_OK)
    {
      if(data==CMD83_CHK_ERROR) 
	return ERR_CHECKSUM;
      else 
	return ERR_INVALID_BYTE;
    }
  TRY(cable->get(&data));
  w=data;
  TRY(cable->get(&data));
  w|=(data << 8);
  if(w != length) return ERR_PACKET;
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

// The TI indicates that it is ready or wait data
// 83 09 00 00
static int ti83_waitdata(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83_WAIT_DATA) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  DISPLAY("The calculator wait data.\n");

  return 0;
}





// Check whether the TI reply that it is ready
int ti83_isready(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

// Send a string of 8 characters
static int sendstring8(char *s, word *checksum)
{
  int i;
  
  for(i=0; i<(int)strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (0xFF & s[i]);      
    }
  for(i=strlen(s); i<8; i++)
    {
      TRY(cable->put(0x00));
    }
  
  return 0;    
}

#define TI83_MAXTYPES 16
const char *TI83_TYPES[TI83_MAXTYPES]=
{
  "REAL", "LIST", "MATRX", "EQU", "STRNG", "PRGM", "ASM", "PIC", 
  "GDB", "??", "??", "??", "CPLX", "??", "??", "??" 
};




const char *TI83_EXT[TI83_MAXTYPES]=
{
  "83n", "83l", "83m", "83y", "83s", "83p", "83p", "83i",
  "83d", "83?", "83?", "83?", "83c", "83?", "83?", "83?"
};





// Return the type corresponding to the value
const char *ti83_byte2type(byte data)
{
  if(data==TI83_DIR) return "DIR";
  else if(data>TI83_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return "??";
    }
  else 
    {
      return TI83_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti83_type2byte(char *s)
{
  int i;
  
  for(i=0; i<TI83_MAXTYPES; i++)
    {
      if(!strcmp(TI83_TYPES[i], s)) break;
    }
  if(i > TI83_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti83_byte2fext(byte data)
{
  if(data > TI83_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return ".83?";
    }
  else 
    {
      return TI83_EXT[data];
    }
}

// Return the value corresponding to the file extension
byte ti83_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI83_MAXTYPES; i++)
    {
      if(!strcmp(TI83_EXT[i], s)) break;
    }
  if(i>TI83_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

/* Oriented packet functions */

static int send_request(word size, byte type, char *string)
{
  byte data;
  word sum;

  sum=0;
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_REQUEST));
  TRY(cable->put(0x0B));
  TRY(cable->put(0x00));
  data=LSB(size);
  sum+=data;
  TRY(cable->put(data));
  data=MSB(size);
  sum+=data;
  TRY(cable->put(data));
  data=type;
  sum+=data;
  TRY(cable->put(type));
  sendstring8(string, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  return 0;
}



























static int receive_var_header(word *size, byte *type, char *string)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int j;

  sum=0;
  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD83_EOT) return ERR_DISCONTINUE;
  else if(data != CMD83_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  j=data;
  TRY(cable->get(&data));
  j|=data << 8;
  TRY(cable->get(&data));
  (*size)=data;
  sum+=data;
  TRY(cable->get(&data));
  (*size) |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  (*type)=data;
  sum+=data;
  for(i=0; i<j-3; i++)
    {
      TRY(cable->get(&data));
      string[i]=data;
      //DISPLAY("%c %02X", data, data);
      sum+=data;
    }
  string[i]='\0';
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  return 0;
}

/* General functions */

int ti83_send_key(int key)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

int ti83_remote_control(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

int ti83_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i;

  TRY(cable->open_port());
  update_start();
  sc->width=TI83_COLS;
  sc->height=TI83_ROWS;
  sc->clipped_width = TI83_COLS;
  sc->clipped_height = TI83_ROWS;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI83_COLS*TI83_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Request screendump.\n");
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  
  TRY(ti83_isOK());
  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83_DATA_PART) return ERR_INVALID_BYTE;
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
  TRY(PC_replyOK_83());
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti83_receive_backup(FILE *file, int mask_mode, longword *version)
{
  byte header[9];
  word block_size;
  byte data;
  word sum;
  word checksum;
  word file_checksum;
  word size;
  int i, j;
  char desc[43]="Backup file received by TiLP";
  long offset;

  TRY(cable->open_port());
  update_start();
  file_checksum=0;
  fprintf(file, "**TI83**");
  fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
  for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
  offset=ftell(file);
  fprintf(file, "XX");
  size=0;
  sum=0;

  DISPLAY("Request backup...\n");
  for(i=0; i<8; i++) { header[i]=0x00; }
  TRY(send_request(0x0000, TI83_BKUP, header));
  TRY(ti83_isPacketOK(0x000B));

  DISPLAY("Receiving backup...\n");  
  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83_VAR_HEADER) return ERR_INVALID_BYTE;
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

  TRY(PC_replyOK_83());
  TRY(PC_waitdata_83());
  TRY(ti83_isOK());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      DISPLAY("Receiving part %i\n", i+1);
      TRY(cable->get(&data));
      if(data != TI83_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD83_DATA_PART) return ERR_INVALID_BYTE;
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

      TRY(PC_replyOK_83());
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
  DISPLAY("Backup complete.\n");
  fseek(file, offset, SEEK_SET);
  size+=2;
  fprintf(file, "%c%c", LSB(size+15), MSB(size+15));
  fseek(file, 0L, SEEK_END);

  update_start();
  TRY(cable->close_port());
  DISPLAY("\n");

  return 0;
}

int ti83_send_backup(FILE *file, int mask_mode)
{
  byte data;
  char str[128];
  word sum;
  int i;
  int j;
  word block_size;

  TRY(cable->open_port());  
  update_start();
  DISPLAY("Sending backup...\n");
  fgets(str, 9, file);

  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
        {
          if( strcmp(str, "**TI82**") && strcmp(str, "**TI83**") )
            {
              return ERR_INVALID_TIXX_FILE;
            }
        }
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
          fprintf(stderr, "MODE_FILE_CHK_ALL\n");
          if( strcmp(str, "**TI83**"))
            {
              return ERR_INVALID_TI83_FILE;
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
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_VAR_HEADER2));
  data=fgetc(file);
  TRY(cable->put(data));
  block_size=data;
  data=fgetc(file);
  TRY(cable->put(data));
  block_size+=data << 8;
  for(i=0; i<9; i++)
    {
      data=fgetc(file);
      sum+=data;
      TRY(cable->put(data));
    }
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  TRY(ti83_isPacketOK(block_size));
  TRY(ti83_waitdata()); 
			
  TRY(PC_replyOK_83());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
      DISPLAY("Sending part %i...\n", i+1);
      sum=0;
      TRY(cable->put(PC_TI83));
      TRY(cable->put(CMD83_DATA_PART));
            
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
      
      TRY(ti83_isPacketOK(block_size));
    }
  TRY(PC_replyOK_83());
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());
    
  return 0;
}

// Current directory list format
int ti83_directorylist(struct varinfo *list, int *n_elts)
{
    byte data;
    int i;
    byte var_type;
    char var_name[9];
    struct varinfo *p;
    word size;
    int err;

    TRY(cable->open_port());
    update_start();
    *n_elts=0;
    p=list;
    p->next=NULL;
    p->folder=NULL;
    strcpy(p->varname, "");
    p->varsize=0;
    p->vartype=0;
    p->varlocked=0;
    strcpy(p->translate, "");

    DISPLAY("Request directory list (dir)...\n");
    for(i=0; i<8; i++) { var_name[i]=0x00; }
    TRY(send_request(0x0000, TI83_DIR, var_name));

    TRY(ti83_isPacketOK(0x000B));

    TRY(cable->get(&data));
    if(data != TI83_PC) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    if(data != CMD83_DATA_PART) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));

    TRY(PC_replyOK_83());

    for( ; ; )
      {
	err = receive_var_header(&size, &var_type, var_name);
	if(err == ERR_DISCONTINUE) break;

	if( (p->next=(struct varinfo *)malloc(sizeof(struct varinfo))) == NULL)
	  {
	    fprintf(stderr, "Unable to allocate memory.\n");
	    exit(0);
	  }
	p=p->next;
	p->next=NULL;
	(*n_elts)++;

	strcpy(p->varname, var_name);
	p->vartype=var_type;
	p->varsize=size;
	p->varlocked=0;
	p->folder=list;
	p->is_folder = VARIABLE;
	strncpy(p->translate, p->varname, 9);
	/* Translate the varname if necessary */
	ti83_translate_varname(p->varname, p->translate, p->vartype);
	
	DISPLAY("Name: %8s | ", p->translate);
	DISPLAY("Type: %8s | ", ti83_byte2type(p->vartype));
	DISPLAY("Locked: %i | ", p->varlocked);
	DISPLAY("Size: %08X\n", p->varsize);

	TRY(PC_replyOK_83());
	sprintf(update->label_text, "Reading of: TI83/%s", p->translate);
	update_label();
	if(update->cancel) return ERR_ABORT;
      }
    DISPLAY("The calculator does not want continue.\n");
    TRY(cable->get(&data));
    TRY(cable->get(&data));

    TRY(PC_replyOK_83());
    DISPLAY("\n");
    TRY(cable->close_port());
    
  return 0;
}

int ti83_receive_var(FILE *file, int mask_mode, 
		     char *varname, byte vartype, byte varlock)
{  
  byte data;
  word sum;
  word checksum;
  static word file_checksum;
  word var_size;
  char name[9];
  byte var_type;
  int i;
  static long offset;
  char trans[9];
  char desc[43]="File received by tilp";
  static word allvars_size;	// This limits the size of a TIGL file to 64 Kb */
  int k;

  update_start();
  TRY(cable->open_port());
  if( (mask_mode & MODE_RECEIVE_FIRST_VAR) || 
      (mask_mode & MODE_RECEIVE_SINGLE_VAR) )
    {
      fprintf(file, "**TI83**");
      fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
      for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
      offset=ftell(file);
      fprintf(file, "XX");
      file_checksum=0;
      allvars_size=0;
    }
  var_size=0;
  sprintf(update->label_text, "Reading of: TI83/%s", 
	   ti83_translate_varname(varname, trans, vartype));
  update_label();
  DISPLAY("Request variable: %s\n", varname);
  TRY(send_request(0x0000, vartype, varname));

  TRY(ti83_isPacketOK(0x000B));
  
  DISPLAY("Receiving variable(s)...\n");  
  sum=0;
  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD83_EOT) return ERR_ABORT; //break;
  else if(data != CMD83_VAR_HEADER) return ERR_INVALID_BYTE;
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
  if(allvars_size+var_size+15 < allvars_size) 
    return ERR_GRP_SIZE_EXCEEDED;
  allvars_size+=var_size+15;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  var_size |= (data << 8);
  sum+=data;
  DISPLAY("-> Size: 0x%04X = %i.\n", var_size, var_size);
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  var_type=data;
  DISPLAY("-> Type: %s\n", ti83_byte2type(data));
  sum+=data;
  for(i=0; i<8; i++)
    {
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      sum+=data;
      name[i]=data;
    }
  name[i]='\0';
  strcpy(varname, name);
  DISPLAY("-> Name: %s <", varname);
  for(k=0; k<8; k++) DISPLAY("%02X", varname[k]);
  DISPLAY(">\n");
  DISPLAY("-> Translated name: %s\n", ti83_translate_varname(varname, trans, var_type));
  file_checksum+=sum;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_83());
  TRY(PC_waitdata_83());
  DISPLAY("The calculator want continue.\n");
  
  TRY(ti83_isOK());
  DISPLAY("Receiving variable...\n");
  TRY(cable->get(&data));
  if(data != TI83_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83_DATA_PART) return ERR_INVALID_BYTE;
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
  
  TRY(PC_replyOK_83());

  if( (mask_mode & MODE_RECEIVE_SINGLE_VAR) || 
      (mask_mode & MODE_RECEIVE_LAST_VAR) )
    {   
      fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
      fseek(file, offset, SEEK_SET);
      if(mask_mode & MODE_RECEIVE_SINGLE_VAR)
	fprintf(file, "%c%c", LSB(var_size+15), MSB(var_size+15));
      else
	if(mask_mode & MODE_RECEIVE_LAST_VAR)
	  fprintf(file, "%c%c", LSB(allvars_size), MSB(allvars_size));
      fseek(file, 0L, SEEK_END);
      DISPLAY("\n");
    }  

  update_start();
  TRY(cable->close_port());
  PAUSE(pause_between_vars);
  
  return 0;
}

int ti83_send_var(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  word block_size;
  longword varsize;
  char varname[9];
  byte vartype;
  int i;
  char trans[9];
  char str[9];
 
  TRY(cable->open_port());
  update_start();
  fgets(str, 9, file);

  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
        {
          if( strcmp(str, "**TI82**") && strcmp(str, "**TI83**") )
            {
              return ERR_INVALID_TIXX_FILE;
            }
        }
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
          fprintf(stderr, "MODE_FILE_CHK_ALL\n");
          if( strcmp(str, "**TI83**"))
            {
              return ERR_INVALID_TI83_FILE;
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
      sprintf(update->label_text, "Variable: %s", 
	       ti83_translate_varname(varname, trans, vartype));
      update_label();
      DISPLAY("Sending variable...\n");
      DISPLAY("-> Name: %s\n", varname);
      DISPLAY("-> Translated name: %s\n", 
	      ti83_translate_varname(varname, trans, vartype));
      DISPLAY("-> Size: %08X\n", varsize);
      DISPLAY("-> Type: %s\n", ti83_byte2type(vartype));
      sum=0;
      TRY(cable->put(PC_TI83));
      TRY(cable->put(CMD83_VAR_HEADER2));
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
      TRY(sendstring8(varname, &sum));
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
    
      TRY(ti83_isPacketOK(block_size));
      TRY(ti83_waitdata()); 
    
      sum=0;
      TRY(PC_replyOK_83());
      TRY(cable->put(PC_TI83));
      TRY(cable->put(CMD83_DATA_PART));
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
      
      TRY(ti83_isPacketOK(block_size));
    
      TRY(cable->put(PC_TI83));
      TRY(cable->put(CMD83_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
    }
  DISPLAY("The computer does not want continue.\n");
  //TRY(ti83_isOK());
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());
    
  return 0;
}

#define DUMP_ROM83_FILE "dumprom.83p"

int ti83_dump_rom(FILE *file, int mask_mode)
{
  int i, j;
  int total;
  byte data;
  time_t start, elapsed, estimated, remaining;
  char buffer[MAXCHARS];
  char tmp[MAXCHARS];
  int pad;
  FILE *f;
  word checksum, sum;
  int err;
  int b=0;;

  update_start();
  sprintf(update->label_text, "Ready ?");
  update_label();

  /* Open connection and check */
  TRY(cable->open_port());
  //TRY(ti83_isready());
  TRY(cable->close_port());
  sprintf(update->label_text, "Yes !");
  update_label();

  /* Transfer ROM dump program from lib to calc */
  f = fopen(DUMP_ROM83_FILE, "wb");
  if(f == NULL)
    return -1;
  fwrite(romDump83, sizeof(unsigned char),
         romDumpSize83, f);
  fclose(f);
  f = fopen(DUMP_ROM83_FILE, "rb");
  TRY(ti83_send_var(f, MODE_NORMAL));
  fclose(f);
  //unlink(DUMP_ROM83_FILE);

  /* As we can not launch program by remote control, we wait user do that */
  sprintf(update->label_text, "Launch from calc...");
  update_label();

  do
    {
      update_refresh();
      if(update->cancel) return ERR_ABORT;
      err = cable->get(&data);
      sum = data;
    }
  while( (err == 35) || (err == 3) );
  fprintf(file, "%c", data);
  
  /* Receive it now blocks per blocks (1024 + CHK) */
  update_start();
  sprintf(update->label_text, "Receiving...");
  update_label();
  start = time(NULL);
  total = 256 * 1024; // 256KB
  update->total = total;

  for(i=0; i<256; i++)
    {
      if(b) sum = 0;
      for (j=0; j<1023+b; j++)
        {
          TRY(cable->get(&data));
          fprintf(file, "%c", data);
          sum += data;
          update->percentage = (float)j/1024;
          update_pbar();
          if(update->cancel) return -1;
        }
      b = 1;
      TRY(cable->get(&data));
      checksum = data << 8;
      TRY(cable->get(&data));
      checksum |= data;
      if(sum != checksum) return ERR_CHECKSUM;
      TRY(cable->put(0xda));

      update->count = 256;
      update->main_percentage = (float)i/(256);
      if(update->cancel) return -1;

      elapsed = (long)difftime(time(NULL), start);
      estimated = (long)(elapsed * (float)(256) / i);
      remaining = (long)difftime(estimated, elapsed);
      sprintf(buffer, "%s", ctime(&remaining));
      sscanf(buffer, "%3s %3s %i %s %i", tmp,
             tmp, &pad, tmp, &pad);
      sprintf(update->label_text, "Remaining (mm:ss): %s", tmp+3);
      update_label();
    }
  /* Close connection */
  TRY(cable->close_port());

  return 0;
}

int ti83_get_rom_version(char *version)
{
  return ERR_VOID_FUNCTION;
}

int ti83_send_flash(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}
