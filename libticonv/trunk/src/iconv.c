/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticonv - charset library, a part of the TiLP project
 *  Copyright (C) 2006-2006 Romain Lievin and Kevin Kofler
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
	This unit contains an iconv-compatible wrapper for the TI charset
	conversion routines.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <iconv.h>
#include <errno.h>

#include "ticonv.h"
#include "charset.h"

#define TIE		TIEXPORT4
#define TIC		TICALL


/* This is a version of tifiles_string_to_model without the "USB" variants
   which don't make much sense for charset use. And we can't use tifiles
   functions here anyway (otherwise we'd have a circular dependency). */
static CalcModel ticonv_string_to_model(const char *str)
{
	if(!g_ascii_strcasecmp(str, "TI73"))
		return CALC_TI73;
	else if(!g_ascii_strcasecmp(str, "TI82"))
		return CALC_TI82;
	else if(!g_ascii_strcasecmp(str, "TI83"))
		return CALC_TI83;
	else if(!g_ascii_strcasecmp(str, "TI83+"))
		return CALC_TI83P;
	else if(!g_ascii_strcasecmp(str, "TI84+"))
		return CALC_TI84P;
	else if(!g_ascii_strcasecmp(str, "TI85"))
		return CALC_TI85;
	else if(!g_ascii_strcasecmp(str, "TI86"))
		return CALC_TI86;
	else if(!g_ascii_strcasecmp(str, "TI89"))
		return CALC_TI89;
	else if(!g_ascii_strcasecmp(str, "TI89t"))
		return CALC_TI89T;
	else if(!g_ascii_strcasecmp(str, "TI92"))
		return CALC_TI92;
	else if(!g_ascii_strcasecmp(str, "TI92+"))
		return CALC_TI92P;
	else if(!g_ascii_strcasecmp(str, "V200"))
		return CALC_V200;
		
	return CALC_NONE;
}

/* Allocate descriptor for code conversion from codeset FROMCODE to
   codeset TOCODE.  */
TIE ticonv_iconv_t TIC ticonv_iconv_open (const char *tocode, const char *fromcode)
{
  ticonv_iconv_t cd;
  cd.src_calc=ticonv_string_to_model(fromcode);
  cd.dest_calc=ticonv_string_to_model(tocode);
  cd.iconv_desc=iconv_open(cd.src_calc?"UTF-16":fromcode,cd.dest_calc?"UTF-16":tocode);
  cd.lookahead=0;
  cd.lossy_count=0;
  return cd;
}

/* Convert at most *INBYTESLEFT bytes from *INBUF according to the
   code conversion algorithm specified by CD and place up to
   *OUTBYTESLEFT bytes in buffer at *OUTBUF.  */
TIE size_t TIC ticonv_iconv (ticonv_iconv_t cd, char **__restrict inbuf,
                             size_t *__restrict inbytesleft,
                             char **__restrict outbuf,
                             size_t *__restrict outbytesleft)
{
  size_t result;
  if (!inbuf || !inbuf) {
    if (!outbuf || !*outbuf || cd.dest_calc) {
      result=iconv(cd.iconv_desc,NULL,NULL,NULL,NULL)+cd.lossy_count;
      cd.lookahead=0;
      cd.lossy_count=0;
      return result;
    } else {
      return iconv(cd.iconv_desc,NULL,NULL,outbuf,outbytesleft)+cd.lossy_count;
      cd.lookahead=0;
      cd.lossy_count=0;
      return result;
    }
  } else {
    unsigned short *temp=NULL;
    void *iconv_src;
    size_t iconv_inbytes;
    size_t bufsize=2;
    char *buf=NULL;
    char *iconv_dest=NULL;

    if (cd.src_calc) {
      char *input=g_malloc(*inbytesleft+1), *inputp=input;
      size_t tempsize;
      strncpy(input,*inbuf,*inbytesleft);
      input[*inbytesleft]=0;
      temp=ticonv_charset_ti_to_utf16(cd.src_calc,input);
      tempsize=ticonv_utf16_strlen(temp)+1;
      for (inputp=input; strchr(inputp,0)<input+*inbytesleft; inputp=strchr(inputp,0)+1) {
        unsigned short *temp2=ticonv_charset_ti_to_utf16(cd.src_calc,inputp);
        size_t temp2size=ticonv_utf16_strlen(temp2)+1;
        temp=g_realloc(temp,(tempsize+temp2size)<<1);
        memcpy(temp+tempsize,temp2,temp2size);
        tempsize+=temp2size;
        g_free(temp2);
      }
      g_free(input);
      iconv_src=temp;
      iconv_inbytes=tempsize<<1;
      if ((*inbuf)[*inbytesleft]) iconv_inbytes-=2;
    } else {
      iconv_src=*inbuf;
      iconv_inbytes=*inbytesleft;
    }

    if (cd.dest_calc) {
      unsigned short *convchar=NULL;
      result=0;
      while(1) {
        size_t iconv_size, iconv_dest_pos, convchar_pos;
        iconv(cd.iconv_desc,NULL,NULL,NULL,NULL);
        iconv_dest_pos=iconv_dest-buf;
        convchar_pos=(char*)convchar-buf;
        buf=g_realloc(buf,bufsize+4);
        iconv_dest=buf+iconv_dest_pos;
        convchar=(unsigned short *)(buf+convchar_pos);
        iconv_size=bufsize-iconv_dest_pos;
        if (cd.lookahead) {
          if (!convchar) {
            *(unsigned short*)iconv_dest=cd.lookahead;
            convchar=(unsigned short *)iconv_dest;
            iconv_dest+=2;
            iconv_size-=2;
          }
          result=cd.lookahead_result;
          if (result==(size_t)-1) errno=cd.lookahead_errno;
          cd.lookahead=0;
        } else {
          if (!convchar) {
            convchar=(unsigned short *)iconv_dest;
          }
          result=iconv(cd.iconv_desc,(void*)&iconv_src,&iconv_inbytes,&iconv_dest,&iconv_size);
        }
        if (result==(size_t)-1 && errno!=E2BIG) {
          break;
        }
        if (result!=(size_t)-1) {
          result+=cd.lossy_count;
          cd.lossy_count=0;
        } else if (iconv_size) { /* no output */
          bufsize+=2;
          continue;
        }
        ((unsigned short *)buf)[bufsize>>1]=0;
        if (!*convchar) {
          if (!*outbytesleft) {
            break;
          }
          *((*outbuf)++)=0;
          (*outbytesleft)--;
          convchar=NULL;
        } else {
          char out[3];
          ticonv_charset_utf16_to_ti_s(cd.dest_calc,convchar,out);
          if (*out=='?' && *convchar!='?') {
            if (!*outbytesleft) {
              break;
            }
            if (result==(size_t)-1) {
              iconv_size+=2; /* look 1 codepoint ahead */
              result=iconv(cd.iconv_desc,(void*)&iconv_src,&iconv_inbytes,&iconv_dest,&iconv_size);
              if (result==(size_t)-1 && errno!=E2BIG) {
                break;
              }
              ticonv_charset_utf16_to_ti_s(cd.dest_calc,convchar,out);
              if (*out=='?') { /* lookahead failed to produce anything */
                if (result!=(size_t)-1 || !iconv_size) { /* only save lookahead if it produced anything */
                  cd.lookahead=convchar[1];
                  cd.lookahead_result=result;
                  if (result==(size_t)-1) cd.lookahead_errno=errno;
                }
                result=(size_t)-1;
                cd.lossy_count++;
                *((*outbuf)++)='?';
                (*outbytesleft)--;
                convchar++;
              } else { /* lookahead succeeded */
                if (result!=(size_t)-1) {
                  result+=cd.lossy_count;
                  cd.lossy_count=0;
                }
                *((*outbuf)++)=*out;
                (*outbytesleft)--;
                convchar=NULL;
              }
            } else {
              cd.lossy_count++;
              *((*outbuf)++)='?';
              (*outbytesleft)--;
              convchar=NULL;
            }
          } else {
            if (!*outbytesleft) {
              break;
            }
            *((*outbuf)++)=*out;
            (*outbytesleft)--;
            convchar=NULL;
          }
        }
        if (result!=(size_t)-1) {
          break;
        }
        bufsize+=2;
      }
      g_free(buf);
    } else {
      result=iconv(cd.iconv_desc,(void*)&iconv_src,&iconv_inbytes,outbuf,outbytesleft);
    }

    if (cd.src_calc) {
      if (iconv_inbytes) {
        char *tmp1=ticonv_charset_utf16_to_ti(cd.src_calc,iconv_src),*tmp2;
        unsigned short *p;
        size_t l1,l2;
        for (p=temp; p<(unsigned short *)iconv_src; p++) {
          if (!*p) *p='_';
        }
        l1=strlen(tmp1);
        g_free(tmp1);
        tmp2=ticonv_charset_utf16_to_ti(cd.src_calc,temp);
        l2=strlen(tmp2);
        g_free(tmp2);
        *inbuf+=l2-l1;
        *inbytesleft-=l2-l1;
      } else {
        *inbuf+=*inbytesleft;
        *inbytesleft=0;
      }
    } else {
      *inbuf=iconv_src;
      *inbytesleft=iconv_inbytes;
    }
    g_free(temp);
    return result;
  }
}

/* Free resources allocated for descriptor CD for code conversion.  */
TIE int TIC ticonv_iconv_close (ticonv_iconv_t cd)
{
  return iconv_close(cd.iconv_desc);
}
