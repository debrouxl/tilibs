/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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
  Thanks to RB for some informations...
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc_err.h"
#include "defs83p.h"
#include "keys83p.h"
#include "calc_ext.h"
#include "trans.h"
#include "pause.h"
#include "rom83p.h"
#include "update.h"
#include "intelhexa.h"

/* Functions used by TI_PC functions */

// The PC indicates that is OK
// 23 56 00 00
static int PC_replyOK_83p(void)
{
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

int PC_replyCONT_83p(void)
{
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_CONTINUE));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

// The PC indicates that it is ready or wait data
// 23 09 00 00
static int PC_waitdata_83p(void)
{
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");
  
  return 0;
}

// Check whether the TI reply OK
// 73 56 00 00
static int ti83p_isOK(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI83p_PC) 
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != CMD83p_TI_OK)
    { 
      if(data==CMD83p_CHK_ERROR) 
	return ERR_CHECKSUM;
      else  if(data==CMD83p_REJECTED)
		  return ERR_VAR_REFUSED;
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

// The TI indicates that it is ready or wait data
// 73 09 00 00
static int ti83p_waitdata(byte id, word length)
{
  byte data;
  word w;

  TRY(cable->get(&data));
  if(data != id) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_WAIT_DATA) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  w=data;
  TRY(cable->get(&data));
  w|=(data << 8);
  if(w != length) return ERR_PACKET;
  DISPLAY("The calculator wait data.\n");

  return 0;
}

// Check whether the TI reply that it is ready
int ti83p_isready(void)
{
  //LOCK_TRANSFER();
  TRY(cable->open());
  DISPLAY("Is calculator ready ?\n");
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_ISREADY));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(ti83p_isOK());
  DISPLAY("The calculator is ready.\n");
  TRY(cable->close());
  //UNLOCK_TRANSFER();

  return 0;
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
#define ti83p_sendstring8 sendstring8

#define TI83p_MAXTYPES 48
const char *TI83p_TYPES[TI83p_MAXTYPES]=
{
  "REAL", "LIST", "MATRX", "Y-VAR", "STRNG", "PRGM", "ASM", "PIC", 
  "GDB", "??", "??", "WDW", "CPLX", "??", "??", "WDW",
  "ZSTO", "TAB", "??", "??", "??", "??", "??", "??",
  "??", "??", "??", "??", "??", "??", "??", "??",
  "??", "??", "??", "AMS", "APPL", "??", "??", "??"
};

const char *TI83p_EXT[TI83p_MAXTYPES]=
{
  "8Xn", "8Xl", "8Xm", "8Xy", "8Xs", "8Xp", "8Xp", "8Xi",
  "8Xd", "8??", "8??", "8Xw", "8Xc", "8??", "8??", "8Xw",
  "8Xz", "8Xt", "8??", "8??", "8??", "8??", "8??", "8??",
  "8??", "8??", "8??", "8??", "8??", "8??", "8??", "8??",
  "8??", "8??", "8??", "8Xu", "8Xk", "8??", "8??", "8??"
};
/* "82w", "82z", "82t" */

// Return the type corresponding to the value
const char *ti83p_byte2type(byte data)
{
  if(data==TI83p_DIR) return "DIR";
  else if(data>=TI83p_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return "??";
    }
  else 
    {
      return TI83p_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti83p_type2byte(char *s)
{
  int i;
  
  for(i=0; i<TI83p_MAXTYPES; i++)
    {
      if(!strcmp(TI83p_TYPES[i], s)) break;
    }
  if(i >= TI83p_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti83p_byte2fext(byte data)
{
  if(data >= TI83p_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return ".8x?";
    }
  else 
    {
      return TI83p_EXT[data];
    }
}

// Return the value corresponding to the file extension
byte ti83p_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI83p_MAXTYPES; i++)
    {
      if(!strcmp(TI83p_EXT[i], s)) break;
    }
  if(i>=TI83p_MAXTYPES)
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
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_REQUEST));
  TRY(cable->put(0x0D));
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
  TRY(cable->put(0x00)); // extra bytes with TI83+ protocol
  TRY(cable->put(0x00));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  return 0;
}

static int recv_var_header(word *size, byte *type, byte *attr, char *string)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int j;

  sum=0;
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD83p_EOT) return ERR_DISCONTINUE;
  else if(data != CMD83p_VAR_HEADER) return ERR_INVALID_BYTE;
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
  for(i=0; i<j-5; i++)
    {
      TRY(cable->get(&data));
      string[i]=data;
      //DISPLAY("%c %02X", data, data);
      sum+=data;
    }
  string[i]='\0';
  TRY(cable->get(&data)); // 2 extra bytes
  TRY(cable->get(attr));
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  return 0;
}

/* General functions */

// It seems that TI83+ has a remote control...
int ti83p_send_key(word key)
{
  //LOCK_TRANSFER();
  TRY(cable->open());
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_DIRECT_CMD));
  TRY(cable->put(LSB(key)));
  TRY(cable->put(MSB(key)));
  TRY(ti83p_isOK());
  TRY(cable->close());
  //UNLOCK_TRANSFER();
    
  return 0;
}


int ti83p_remote_control(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

int ti83p_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i;

  //LOCK_TRANSFER();
  TRY(cable->open());
  update_start();
  sc->width=TI83p_COLS;
  sc->height=TI83p_ROWS;
  sc->clipped_width = TI83p_COLS;
  sc->clipped_height = TI83p_ROWS;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI83p_COLS*TI83p_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Request screendump.\n");
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  
  TRY(ti83p_isOK());
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
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
  TRY(PC_replyOK_83p());
  DISPLAY("\n");

  update_start();
  TRY(cable->close());
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_recv_backup(FILE *file, int mask_mode, longword *version)
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

  return ERR_VOID_FUNCTION;
  //LOCK_TRANSFER();
  TRY(cable->open());
  update_start();
  file_checksum=0;
  fprintf(file, "**TI83F*");
  fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
  for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
  offset=ftell(file);
  fprintf(file, "XX");
  size=0;
  sum=0;

  DISPLAY("Request backup...\n");
  for(i=0; i<8; i++) { header[i]=0x00; }
  TRY(send_request(0x0000, TI83p_BKUP, header));
  TRY(ti83p_isOK());

  DISPLAY("Receiving backup...\n");  
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  j=data; /* j should be equal to 0x09 but to be the safe side ... */
  if(data != 0x09) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
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

  TRY(PC_replyOK_83p());
  TRY(PC_waitdata_83p());
  TRY(ti83p_isOK());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      DISPLAY("Receiving part %i\n", i+1);
      TRY(cable->get(&data));
      if(data != TI83p_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
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
	  (update->percentage)=(float)j/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(sum != checksum) return ERR_CHECKSUM;

      TRY(PC_replyOK_83p());
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
  TRY(cable->close());
  DISPLAY("\n");
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_send_backup(FILE *file, int mask_mode)
{
  byte data;
  char str[128];
  word sum;
  int i;
  int j;
  word block_size;

  return ERR_VOID_FUNCTION;
  //LOCK_TRANSFER();
  TRY(cable->open());  
  update_start();
  DISPLAY("Sending backup...\n");
  fgets(str, 9, file);
  
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
        {
          if( strcmp(str, "**TI83**") && strcmp(str, "**TI83F*") )
            {
              return ERR_INVALID_TIXX_FILE;
            }
        }
      else if(mask_mode & MODE_FILE_CHK_ALL)
        {
          fprintf(stderr, "MODE_FILE_CHK_ALL\n");
          if( strcmp(str, "**TI83F*"))
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
  printf("data=0x%02x\n", data);
  data=fgetc(file);
  printf("data=0x%02x\n", data);

  sum=0;
  DISPLAY("Sending header.\n");
  TRY(cable->put(TI83p_PC));
  TRY(cable->put(CMD83p_VAR_HEADER2));
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

  TRY(ti83p_isOK());
  TRY(ti83p_waitdata(TI83p_PC, block_size)); 

  TRY(PC_replyOK_83p());
  for(i=0; i<3; i++)
    {
      (update->main_percentage)=(float)i/3;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
      DISPLAY("Sending part %i...\n", i+1);
      sum=0;
      TRY(cable->put(TI83p_PC));
      TRY(cable->put(CMD83p_DATA_PART));
            
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
      
      TRY(ti83p_isOK());
    }
  TRY(PC_replyOK_83p());
  DISPLAY("\n");

  update_start();
  TRY(cable->close());
  //UNLOCK_TRANSFER();
    
  return 0;
}

// Current directory list format
int ti83p_directorylist(struct varinfo *list, int *n_elts)
{
  byte data;
  int i;
  byte var_type;
  char var_name[9];
  struct varinfo *p;
  word size;
  int err;
  byte attr;
  word sum, checksum;

	//LOCK_TRANSFER();
    TRY(cable->open());
    update_start();
    *n_elts=0;
    p=list;
    p->next=NULL;
    p->folder=p;
    strcpy(p->varname, "");
    p->varsize=0;
    p->vartype=0;
    p->varattr=0;
    strcpy(p->translate, "");

    DISPLAY("Request directory list (dir)...\n");
    for(i=0; i<8; i++) { var_name[i]=0x00; }
    TRY(send_request(0x0000, TI83p_DIR, var_name));

    TRY(ti83p_isOK());

    sum=0;
    TRY(cable->get(&data));
    if(data != TI83p_PC) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    list->varsize = data;         // store mem free
    sum+=data;
    TRY(cable->get(&data));
    list->varsize |= (data << 8);
    sum+=data;
    TRY(cable->get(&data));
    checksum=data;
    TRY(cable->get(&data));
    checksum += (data << 8);
    if(checksum != sum) return ERR_CHECKSUM;

    TRY(PC_replyOK_83p());

    for( ; ; )
      {
	err=recv_var_header(&size, &var_type, &attr, var_name);
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
	if(attr == 0x80) p->varattr = VARATTR_ARCH; else p->varattr = VARATTR_NONE;
	p->folder=p;
	p->is_folder = VARIABLE;
	strncpy(p->translate, p->varname, 9);
	/* Translate the varname if necessary */
	ti83_translate_varname(p->varname, p->translate, p->vartype);
	
	DISPLAY("Name: %8s | ", p->translate);
	DISPLAY("Type: %8s | ", ti83p_byte2type(p->vartype));
	DISPLAY("Attr: %i | ", p->varattr);
	DISPLAY("Size: %08X\n", p->varsize);

	TRY(PC_replyOK_83p());
	sprintf(update->label_text, "Reading of: %s", p->translate);
	update_label();
	if(update->cancel) return ERR_ABORT;
      }
    DISPLAY("The calculator does not want continue.\n");
    TRY(cable->get(&data));
    TRY(cable->get(&data));

    TRY(PC_replyOK_83p());
    DISPLAY("\n");
    TRY(cable->close());
    //UNLOCK_TRANSFER();
    
  return 0;
}

int ti83p_recv_var(FILE *file, int mask_mode, 
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

  //LOCK_TRANSFER();
  update_start();
  TRY(cable->open());
  if( (mask_mode & MODE_RECEIVE_FIRST_VAR) || 
      (mask_mode & MODE_RECEIVE_SINGLE_VAR) )
    {
      fprintf(file, "**TI83F*");
      fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
      for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
      offset=ftell(file);
      fprintf(file, "XX");
      file_checksum=0;
      allvars_size=0;
    }
  var_size=0;
  sprintf(update->label_text, "Reading of: %s", 
	   ti83_translate_varname(varname, trans, vartype));
  update_label();
  /* [X91] anglais: inversion */
  DISPLAY("Variable requested: %s\n", varname);
  TRY(send_request(0x0000, vartype, varname));

  TRY(ti83p_isOK());
  
  DISPLAY("Receiving variable(s)...\n");  
  sum=0;
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD83p_EOT) return ERR_ABORT; //break;
  else if(data != CMD83p_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != 0x0D) return ERR_INVALID_BYTE;
  fprintf(file, "%c", data);
  TRY(cable->get(&data));
  if(data != 0x00) return ERR_INVALID_BYTE;
  fprintf(file, "%c", data);
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
  DISPLAY("-> Type: %s\n", ti83p_byte2type(data));
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
  /* [X91] C'est ici que les deux octets de plus vont, c'est le numero
   * de compatibilité du système de fichiers, pour le moment, c'est 00 00
   * (ROM v1.12) */
  TRY(cable->get(&data));
  sum+=data;
  fprintf(file, "%c", data);
  TRY(cable->get(&data));
  sum+=data;
  fprintf(file, "%c", data);

  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_83p());
  TRY(PC_waitdata_83p());
  DISPLAY("The calculator wants to continue.\n");
  
  TRY(ti83p_isOK());
  DISPLAY("Receiving variable...\n");
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  sum+=data;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  sum+=data;
  sum=0;
  update->total = var_size;
  for(i=0; i<var_size; i++)
    {
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      sum+=data;
      
      update->count = i;
      (update->percentage)=(float)i/var_size;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_83p());

  if( (mask_mode & MODE_RECEIVE_SINGLE_VAR) || 
      (mask_mode & MODE_RECEIVE_LAST_VAR) )
    {   
      fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
      fseek(file, offset, SEEK_SET);
      if(mask_mode & MODE_RECEIVE_SINGLE_VAR)
	/* [X91] les deux octets de compatibilité se repercutent ici */
	/* [X91] fprintf(file, "%c%c", LSB(var_size+15), MSB(var_size+15));*/
	fprintf(file, "%c%c", LSB(var_size+17), MSB(var_size+17));
      else
	if(mask_mode & MODE_RECEIVE_LAST_VAR)
	  fprintf(file, "%c%c", LSB(allvars_size), MSB(allvars_size));
      fseek(file, 0L, SEEK_END);
      DISPLAY("\n");
    }  

  update_start();
  TRY(cable->close());
  PAUSE(PAUSE_BETWEEN_VARS);
  //UNLOCK_TRANSFER();
  
  return 0;
}


/* [X91] d'après ce que j'ai lu, la difference entre les
 * fichiers est gérée  (mais de manière laxiste:
 * la détection 0x0D / 0x0B se fait au niveau
 * de l'en tête **TI83** / **TI83F* */
int ti83p_send_var(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  word block_size;
  longword varsize;
  char varname[9];
  byte vartype;
  int i;
  char trans[9]; int fti83p = 1;
  byte varattr = VARATTR_NONE;
  
  //LOCK_TRANSFER();
  TRY(cable->open());
  update_start();
  fgets(trans, 9, file);
  
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
        {
          if( strcmp(trans, "**TI83**") && strcmp(trans, "**TI83F*") &&
			  strcmp(trans, "**TI82**") && strcmp(trans, "**TI85**"))
            {
              return ERR_INVALID_TIXX_FILE;
            }
        }
      else if(mask_mode & MODE_FILE_CHK_ALL)
        {
	  if( strcmp(trans, "**TI83F*"))
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
      if(feof(file)) break;
      if(!strcmp(trans,  "**TI82**") || 
		  !strcmp(trans, "**TI83**") ||
		  !strcmp(trans, "**TI85**")) fti83p = 1; else fti83p = 0;
      //if(data == 0x0d) fti83p = 1; else if(data == 0x0b) fti83p = 0;
      //else break;
      data=fgetc(file);
      if(feof(file)) break;
      if(data != 0) break;
      varsize=fgetc(file);
      if(feof(file)) break;
      varsize+=fgetc(file) << 8;
      vartype=fgetc(file);
      for(i=0; i<8; i++) varname[i]=fgetc(file);
      varname[i]='\0';
      varattr = 0;
      if(fti83p) { // TI83 compatibility
	data = fgetc(file);
	varattr = fgetc(file);
      }
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
      DISPLAY("-> Type: %s\n", ti83p_byte2type(vartype));
      sum=0;
      TRY(cable->put(PC_TI83p));
      TRY(cable->put(CMD83p_VAR_HEADER2));
      block_size = 0x0D;
      //if(fti83p) block_size=0x0D; else block_size=0x0B;
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
      TRY(cable->put(0x00));
      if( (mask_mode & MODE_USE_2ND_HEADER) ||	// if backup
	  (mask_mode & MODE_KEEP_ARCH_ATTRIB) )	// if extended file format
	{
	  //DISPLAY("attr = %02X\n", varattr);
	  TRY(cable->put(varattr));
	  sum+=varattr;
	}
      else
	{
	  TRY(cable->put(0x00));
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti83p_isOK());
      TRY(ti83p_waitdata(TI83p_PC, block_size));

      sum=0;
      TRY(PC_replyOK_83p());
      TRY(cable->put(TI83p_PC));
      TRY(cable->put(CMD83p_DATA_PART));
      block_size=(word)varsize;
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

      TRY(ti83p_isOK());

      DISPLAY("The computer does want to continue.\n");
      TRY(cable->put(PC_TI83p));
      TRY(cable->put(CMD83p_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
    }

  DISPLAY("\n");
  update_stop();
  TRY(cable->close());
  //UNLOCK_TRANSFER();;

  return 0;
}

#define DUMP_ROM83p_FILE "dumprom.83p"

int ti83p_dump_rom(FILE *file, int mask_mode)
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

  //LOCK_TRANSFER();
  update_start();
  sprintf(update->label_text, "Ready ?");
  update_label();

  /* Open connection and check */
  TRY(cable->open());
  //TRY(ti83p_isready());
  TRY(cable->close());
  sprintf(update->label_text, "Yes !");
  update_label();

  /* Transfer ROM dump program from lib to calc */
  f = fopen(DUMP_ROM83p_FILE, "wb");
  if(f == NULL)
    return -1;
  fwrite(romDump83p, sizeof(unsigned char),
         romDumpSize83p, f);
  fclose(f);
  f = fopen(DUMP_ROM83p_FILE, "rb");
  TRY(ti83p_send_var(f, MODE_NORMAL));
  fclose(f);
  //unlink(DUMP_ROM83p_FILE);

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
  total = 256 * 1024;
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
          if(update->cancel) return ERR_ABORT;
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
      if(update->cancel) return ERR_ABORT;

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
  TRY(cable->close());
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_get_rom_version(char *version)
{
  return ERR_VOID_FUNCTION;
}

int ti83p_send_flash(FILE *file, int mask_mode)
{
  byte data;
  word sum, checksum;
  char str[128];
  longword flash_size;
  longword block_size;
  int i, j, ret;
  int num_blocks;
  byte str_size;
  char date[5];
  char *signature = "Advanced Mathematics Software";
  int tib = 0;
  word page_offset = 0x4000;
  word page_number = 0x0000;
  byte flag = 0x80;
  byte buf[0x100];
  
  /* Read the file header and initialize some variables */
  //LOCK_TRANSFER();
  TRY(cable->open());
  update_start();
  fgets(str, 128, file);
  if(strstr(str, "**TIFL**") == NULL) // is a .83pu file
    {
      for(i=0, j=0; i<127; i++) // is a .tib file
	{
	  if(str[i] == signature[j])
	    {
	      j++;
	      if(j==strlen(signature))
		{
		  DISPLAY("TIB file.\n");
		  tib = 1;
		  break;
		}
	    }
	}
      if(j < strlen(signature))
	return ERR_INVALID_FLASH_FILE; // not a FLASH file
    }

  rewind(file);
  if(!tib)
    {
      fgets(str, 9, file);
      
      for(i=0; i<4; i++) 
	fgetc(file);
      
      for(i=0; i<4; i++)
	date[i] = fgetc(file);
      DISPLAY("Date of the FLASHapp or License: %02X/%02X/%02X%02X\n", 
	      date[0], date[1], date[2], 0xff & date[3]);
      str_size=fgetc(file);
      for(i=0; i<str_size; i++)
	str[i]=fgetc(file);
      str[i]='\0';
      for(i=16+str_size+1; i<0x4A; i++)
	fgetc(file);
      flash_size=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
	(LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
      
      if(!strcmp(str, "License"))
	{
	  DISPLAY("There is a license header: skipped.\n");
	  for(i=0; i<flash_size; i++)
	    fgetc(file);
	  
	  fgets(str, 9, file);
	  if(strcmp(str, "**TIFL**"))
	    return ERR_INVALID_FLASH_FILE;
	  for(i=0; i<4; i++) fgetc(file);
	  for(i=0; i<4; i++)
	    date[i] = 0xff & fgetc(file);
	  DISPLAY("Date of the FLASHapp or License: %02X/%02X/%02X%02X\n", 
		  date[0], date[1], date[2], 0xff & date[3]);
	  str_size=fgetc(file);
	  for(i=0; i<str_size; i++)
	    str[i]=fgetc(file);
	  str[i]='\0';
	  for(i=16+str_size+1; i<0x4A; i++)
	    fgetc(file);
	  flash_size=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
	    (LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
	} 
    }
  else
    {
      fseek(file, 0, SEEK_END);
      flash_size = ftell(file);
      fseek(file, 0, SEEK_SET);
      strcpy(str, "basecode");
    }      

  DISPLAY("\n");
  DISPLAY("Sending FLASH application...\n");
  DISPLAY("FLASH application name: \"%s\"\n", str);
  DISPLAY("FLASH application/Operating System size: %i bytes.\n", flash_size);

  /* Now, read data from the file and send them by block */
  if(mask_mode & MODE_AMS)
  {
    block_size = 0x0100;
  }
  else
  {
    block_size = 0x0080;
  }
  
  // The number of block is an approximation... It can not be exactly calculated.
  // Moreover, the size of FLASH app can not be known at this time.
  num_blocks = (flash_size/77)/(block_size>>5); // IntelHexa: 32 bytes encoded as 77 chars
  DISPLAY("Approximative number of blocks: %i\n", num_blocks);

//#define TEST
#ifdef TEST	// Display data on screen (and in the console.log file, if console enabled)
	DISPLAY("!!!\n\n\n");

	ret = j = -1;
		DISPLAY("result = %i\n", ret);
		DISPLAY("%i: flag = 0x%02x\n", j, flag & 0xff);
		for(i=0; i<block_size; i++) DISPLAY("%02X ", buf[i]);
		DISPLAY("\n");

		DISPLAY("result = %i\n", ret);
		DISPLAY("%i: flag = 0x%02x\n", j, flag & 0xff);
		for(i=0; i<block_size; i++) DISPLAY("%02X ", buf[i]);
		DISPLAY("\n");


	read_data_block(file, &page_offset, &page_number, NULL, 0);	// reset block reader
	flag = 0x80;
	j=0;
	while(1)
	//for(j=0; j<2; j++)
	{
		ret = read_data_block(file, &page_offset, &page_number, buf, mask_mode);
		if(j == 1) flag = 0x00;			// first block -> FLASH
		if(ret == 3) flag = 0x80;	// last block -> FLASH
		if(ret < 0) 
		{
			DISPLAY("Exiting...\n");
			break;
		}

		DISPLAY("result = %i\n", ret);
		DISPLAY("%i: flag = 0x%02x\n", j, flag & 0xff);
		for(i=0; i<block_size; i++) DISPLAY("%02X ", buf[i]);
		DISPLAY("\n");
		j++;
	}
	return -1;
#endif

  if(mask_mode & MODE_APPS)
  {
	  // send FLASH app
	TRY(cable->put(PC_TI83p));
	TRY(cable->put(CMD83p_SEND_FLASH));
	TRY(cable->put(0x00));
	TRY(cable->put(0x00));
  
	TRY(ti83p_isOK());
	TRY(PC_waitdata_83p());
	TRY(ti83p_isOK());
  
	sum=0;
	TRY(cable->get(&data));
	if(data != TI83p_PC) return ERR_INVALID_BYTE;
	TRY(cable->get(&data));
	if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
	TRY(cable->get(&data));
	if(data != 0x0B) return ERR_INVALID_BYTE;
	TRY(cable->get(&data));
	if(data != 0x00) return ERR_INVALID_BYTE;
	for(i=0; i<9; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
    }
	TRY(cable->get(&data));
	sum+=data;
	TRY(cable->get(&data));
	sum+=data;
  
	TRY(cable->get(&data));
	checksum=data;
	TRY(cable->get(&data));
	checksum += (data << 8);
	if(checksum != sum) return ERR_CHECKSUM;
  
	TRY(PC_replyOK_83p());
  }
 
  read_data_block(file, &page_offset, &page_number, NULL, 0);	// reset block reader
  flag = 0x80; // OS only
  for(i=0; ;i++)
    {
		ret = read_data_block(file, &page_offset, &page_number, buf, mask_mode);
		if(mask_mode & MODE_AMS)
		{
			if(i == 1)
			{
				flag = 0x00;			// first block -> FLASH
				DISPLAY("Send OS header information...\n");
				sprintf(update->label_text, "Send OS header information.");
				update_label();
			}
			if(ret == 3)
			{
				DISPLAY("Waits that calc displays 'Validating software...'\n");
				sprintf(update->label_text, "Waiting...");
				update_label();
				PAUSE(1000);			// This pause is REQUIRED !!!
				DISPLAY("Send digital signature...\n");
				sprintf(update->label_text, "Send digital signature.");
				update_label();
				flag = 0x80;			// last block -> FLASH
			}
		}
		if(ret < 0)
			break;

		DISPLAY("Sending block %i.\n", i);
      
		sum=0;
		TRY(cable->put(PC_TI83p));
		TRY(cable->put(CMD83p_VAR_HEADER));
		TRY(cable->put(LSB(0x000A)));
		TRY(cable->put(MSB(0x000A)));
		data=block_size & 0x000000FF;
		sum+=data;
		TRY(cable->put(data));
		data=(block_size & 0x0000FF00) >> 8;
		sum+=data;
		TRY(cable->put(data));
      
      if(mask_mode & MODE_AMS)
		data = TI83p_AMS;
      else
		data = TI83p_FLASH;
      TRY(cable->put(data));
      sum+=data;
      
      data=(block_size & 0x00FF0000) >> 8;
      sum+=data;
      TRY(cable->put(data));
      data=(block_size & 0xFF000000) >> 8;
      sum+=data;
      TRY(cable->put(data));
      
	  if(mask_mode & MODE_APPS)
	  {
		TRY(cable->put(0xFF));		//flag
		sum+=0xff;
	  }
	  else if(mask_mode & MODE_AMS)
	  {
		TRY(cable->put(flag));		//flag: 0x80 (OS header), 0x00 (data), 0x80 (signature)
		sum+=flag;
		DISPLAY("flag=%02X\n", flag);
	  }
      
      data=LSB(page_offset);	// Page offset
      TRY(cable->put(data));
      sum+=data;
      data=MSB(page_offset);
      TRY(cable->put(data));
      sum+=data;
      
      data=LSB(page_number);		// Page number
      TRY(cable->put(data));
      sum+=data;
      data=MSB(page_number);
      TRY(cable->put(data));
      sum+=data;
      
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti83p_isOK());
      TRY(ti83p_waitdata(TI83p_PC, 0x000A));
      
      TRY(PC_replyOK_83p());
      
      sum=0;
      TRY(cable->put(PC_TI83p));
      TRY(cable->put(CMD83p_DATA_PART));
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      DISPLAY("Transmitting data.\n");
      update->total = block_size;
      for(j=0; j<block_size; j++)
	  {
		TRY(cable->put(buf[j]));
		sum+=buf[j];
	    
	    update->count = j;
	    update->percentage = (float)j/block_size;
	    update_pbar();
	    if(update->cancel) return ERR_ABORT;
	  }

      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti83p_isOK());
      
      ((update->main_percentage))=(float)i/num_blocks;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  
  TRY(cable->put(PC_TI83p));
  TRY(cable->put(CMD83p_EOT));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  
  if(mask_mode & MODE_APPS)
    DISPLAY("Flash application sent completely.\n");
  else if(mask_mode & MODE_AMS)
	DISPLAY("Flash OS upgrade sent completely.\n");
  DISPLAY("\n");

  update_start();
  TRY(cable->close());
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_recv_flash(FILE *file, int mask_mode, char *appname, word appsize)
{
  byte data;
  word sum;
  word checksum;
  word block_size;
  int i,j,k;
  char *varname = appname;
  word page_address=0;
  word page_number=0;
  byte type;
  byte buffer[0x80];
  long cur_pos, end_pos, pos;

  //LOCK_TRANSFER();
  fprintf(file, "**TIFL**");
  for(i=0; i<4; i++)
    fputc(0x00, file);
  for(i=0; i<4; i++) //date: 24 01 2000
    fputc(0x00, file);
  fputc(strlen(appname), file);
  fprintf(file, "%s", appname);
  for(i=16+strlen(appname)+1; i<0x4A; i++)
    fputc(0x00, file);

  cur_pos = ftell(file);
  fputc(LSB(LSW(0)), file);
  fputc(MSB(LSW(0)), file);
  fputc(LSB(MSW(0)), file);
  fputc(MSB(MSW(0)), file);

  TRY(cable->open());
  update->start();
  sprintf(update->label_text, "Application: %s", varname);
  update->label();
  sum=0;
  DISPLAY("Request FLASH application: %s\n", varname);

  TRY(cable->put(TI83p_PC));
  TRY(cable->put(CMD83p_REQUEST));
  block_size=0x000B;
  data=LSB(block_size);
  TRY(cable->put(data));
  data=MSB(block_size);
  TRY(cable->put(data));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  data=TI83p_FLASH;
  TRY(cable->put(data));
  sum+=data;
  TRY(ti83p_sendstring8(varname, &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti83p_isOK());

  DISPLAY("Receiving application...\n");
  update->total = 5*appsize;
  DISPLAY("total=%i\n", update->total);
  for(k=0; ;k+=block_size)
  {
	sum=0;
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD83p_EOT) break;
  else if(data != CMD83p_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  j=data;
  TRY(cable->get(&data));
  j|=data << 8;
  TRY(cable->get(&data));
  block_size=data;
  sum+=data;
  TRY(cable->get(&data));
  block_size |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  type=data;
  sum+=data;
  for(i=0; i<3; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
    }
  //string[i]='\0';
  TRY(cable->get(&data));
  page_address = data;
  sum+=data;
  TRY(cable->get(&data));
  page_address |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  page_number = data;
  sum+=data;
  TRY(cable->get(&data));
  page_number |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

      TRY(PC_replyOK_83p());
      TRY(PC_waitdata_83p());
      DISPLAY("The calculator want to continue.\n");
      
      TRY(ti83p_isOK());

      DISPLAY("Receiving block (page_offset=%04X, page_number=%04X)...\n", 
		  page_address, page_number);
      TRY(cable->get(&data));
      if(data != TI83p_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      block_size = data;
      TRY(cable->get(&data));
      block_size |= (data << 8);
      sum=0;
      for(j=0; j<block_size; j++)
	{
	  TRY(cable->get(&data));
	  sum+=data;
	  buffer[j] = data;
	  //fprintf(file, "%c", data);
	  
	  update->count = j;
	  update->percentage = (float)j/block_size;
	  update->pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;

	  write_data_block(file, page_address, page_number & 0x00ff, buffer, 0);
      
      TRY(PC_replyOK_83p());
 
      ((update->main_percentage))=(float)k/(update->total);
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
      TRY(PC_replyOK_83p());
      DISPLAY("\n");

	  write_data_block(file, page_address, page_number & 0x00ff, buffer, !0);

	  end_pos = ftell(file);
	  fseek(file, cur_pos, SEEK_SET);
	  DISPLAY("end_pos = 0x%08X\n", end_pos);
	  pos = end_pos - cur_pos - 4;
		fputc(LSB(LSW(pos)), file);
	fputc(MSB(LSW(pos)), file);
	fputc(LSB(MSW(pos)), file);
	fputc(MSB(MSW(pos)), file);
	  fseek(file, 0, SEEK_END);      

  update->start();
  TRY(cable->close());
  PAUSE(PAUSE_BETWEEN_VARS);
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_get_idlist(char *id)
{
  byte data;
  word sum;
  word checksum;
  word block_size;
  int i, j;
  longword var_size;
  byte name_length;
  char name[9];

  //LOCK_TRANSFER();
  TRY(cable->open());
  update_start();
  sum=0;
  DISPLAY("Request IDlist...\n");
  TRY(cable->put(TI83p_PC));
  TRY(cable->put(CMD83p_REQUEST));
  block_size=0x0B;
  data=LSB(block_size);
  TRY(cable->put(data));
  data=MSB(block_size);
  TRY(cable->put(data));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  data=TI83p_IDLIST;
  TRY(cable->put(data));
  sum+=data;
  for(j=0; j<8; j++) TRY(cable->put(0x00)); // null padded name
  //TRY(cable->put(0x00)); // 2 extra bytes
  //TRY(cable->put(0x00));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti83p_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_VAR_HEADER) 
  {
	  if(data != CMD83p_REJECTED)
		return ERR_INVALID_BYTE;
	  else
	  {
		  byte rej_code = CMD83p_REJ_NONE;
			DISPLAY("Var rejected... ");
		TRY(cable->get(&data));
		TRY(cable->get(&data));

		TRY(cable->get(&data));
		TRY(cable->get(&data));
		TRY(cable->get(&data));
		TRY(cable->get(&data));
		rej_code = data;
		//DISPLAY("Rejection code: %02X\n", data);
		TRY(cable->get(&data));
		TRY(cable->get(&data));
		return ERR_NO_IDLIST; //return ERR_VAR_REFUSED;
	  }
  }
	  
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  var_size=data;
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 16);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 24);
  sum+=data;
  DISPLAY("Size of the IDlist: 0x%08X = %i.\n", var_size-2, var_size-2);
  TRY(cable->get(&data));
  DISPLAY("Type of the variable: %s\n", ti83p_byte2type(data));
  sum+=data;
  TRY(cable->get(&data));
  name_length=data;
  sum+=data;
  DISPLAY("Variable name: ");
  for(i=0; i<name_length; i++)
    {
      TRY(cable->get(&data));
      name[i]=data;
      DISPLAY("%c", data);
      sum+=data;
    }
  name[i]='\0';
  DISPLAY("\n");
  TRY(cable->get(&data));
  sum+=data;
  TRY(cable->get(&data)); // 2 extra bytes
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_83p());
  TRY(PC_waitdata_83p());
  DISPLAY("The calculator want continue.\n");

  TRY(ti83p_isOK());
  DISPLAY("Receiving IDlist...\n");
  TRY(cable->get(&data));
  if(data != TI83p_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD83p_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  sum=0;
  for(i=0; i<4; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
    }
  TRY(cable->get(&data));
  block_size = (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  block_size |= data;
  sum+=data;
  DISPLAY("block_size=%x, var_size=%x\n", block_size, var_size);
  block_size=var_size-2; //diff here
  for(i=0, j=0; i<block_size; i++, j++)
    {
      TRY(cable->get(&data));
      sum+=data;
      if( (j == 7) || (j == 13) )
	{
	  id[j]='-';
	  j++;
	}
      id[j]=data;
    }
  id[j]='\0';
  DISPLAY("IDlist: <%s>\n", id+2);
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_83p());
  DISPLAY("\n");
  (update->percentage)=0.0;
  TRY(cable->close());
  //UNLOCK_TRANSFER();

  return 0;
}

int ti83p_supported_operations(void)
{
  return 
    (
     OPS_ISREADY |
     OPS_SCREENDUMP |
	 OPS_SEND_KEY |
     OPS_DIRLIST |
     OPS_SEND_VARS | OPS_RECV_VARS |
     OPS_SEND_FLASH | OPS_RECV_FLASH |
     OPS_IDLIST
     );
}
