/*
 * TI Calculator ROM Dumper
 * 
 * Copyright (c) 2005 Benjamin Moody
 * 
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR
 * ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ticables.h>

#define CMD_PING 0xAA55
#define CMD_NOK 0
#define CMD_OK 1
#define CMD_EOT 2
#define CMD_SIZE 3
#define CMD_RETRY 4
#define CMD_REQ 5
#define CMD_DATA 6
#define CMD_REPEAT 7

typedef unsigned int word;
typedef unsigned char byte;

int getPacket(word *cmd, word *length, byte **buf);
int putPacket(word cmd, word length, byte *buf);
int putGetPacket(word scmd, word slen, byte *sbuf,
		 word *rcmd, word *rlen, byte **rbuf);

int getByte(byte *b);
int putByte(byte b);
int getWord(word *w);
int putWord(word w);

TicableLinkCable lc;
int checksum;

void resync()
{
  int status;
  int rcmd, rlen;
  byte *rbuf;

  fprintf(stderr,"**Resync**\n");

  do {
    status = putPacket(CMD_PING, 0, NULL);
    if (status==0) {
      rbuf = NULL;
      status = getPacket(&rcmd, &rlen, &rbuf);
      if (rbuf)
	free(rbuf);
    }
    else {
      sleep(5);
    }
  } while (status != 0 || rcmd != CMD_OK || rlen != 0);
}

int tcerr(int e)
{
  static char buf[256];

  if (e) {
    ticable_get_error(e, buf);
    fprintf(stderr,"ticables error: %s\n", buf);
  }

  return e;
}


struct linkinfo {
  char *name;
  TicableType t;
  TicablePort p[4];
}
li[]={{"tgl", LINK_TGL, {SERIAL_PORT_1, SERIAL_PORT_2, SERIAL_PORT_3, SERIAL_PORT_4}},
      {"ser", LINK_SER, {SERIAL_PORT_1, SERIAL_PORT_2, SERIAL_PORT_3, SERIAL_PORT_4}},
      {"par", LINK_PAR, {PARALLEL_PORT_1, PARALLEL_PORT_2, PARALLEL_PORT_3, NULL_PORT}},
      {"avr", LINK_AVR, {SERIAL_PORT_1, SERIAL_PORT_2, SERIAL_PORT_3, SERIAL_PORT_4}},
      {"vtl", LINK_VTL, {VIRTUAL_PORT_1, VIRTUAL_PORT_2, NULL_PORT, NULL_PORT}},
      {"tie", LINK_TIE, {VIRTUAL_PORT_1, VIRTUAL_PORT_2, NULL_PORT, NULL_PORT}},
      {"vti", LINK_VTI, {VIRTUAL_PORT_1, VIRTUAL_PORT_2, NULL_PORT, NULL_PORT}},
      {"slv", LINK_SLV, {USB_PORT_1, USB_PORT_2, USB_PORT_3, USB_PORT_4}}};

#define NLINKS 8

void usage(char *progname)
{
  int i;

  fprintf(stderr, "usage: %s ", progname);
  for (i=0; i<NLINKS; i++)
    fprintf(stderr, "%c%s", i?'|':'{', li[i].name);
  fprintf(stderr, "} portnum romfile\n");
}

int main(int argc, char **argv)
{
  TicableLinkParam lp;
  unsigned long int romSize, currentPos;
  int i, linknum=-1, portnum;
  FILE *rfile;

  word rcmd, rlen;
  byte *rbuf = NULL;
  byte cbuf[4];
  int replen;

  if (argc != 4 || !sscanf(argv[2],"%i",&portnum) || portnum<1 || portnum>4) {
    usage(argv[0]);
    return 1;
  }

  for (i=0; i<NLINKS; i++)
    if (0==strcasecmp(li[i].name, argv[1]))
      linknum=i;

  if (linknum==-1) {
    usage(argv[0]);
    return 1;
  }

  rfile = fopen(argv[3],"wb");
  if (!rfile) {
    perror(argv[1]);
    return 1;
  }

  ticable_init();

  tcerr(ticable_get_default_param(&lp));
  lp.link_type = li[linknum].t;
  lp.port = li[linknum].p[portnum-1];
  tcerr(ticable_set_param(&lp));
  if (tcerr(ticable_set_cable(li[linknum].t, &lc)))
    return 1;

  if (tcerr(lc.init() || lc.open()))
    return 1;

  resync();

  do {
    putGetPacket(CMD_SIZE, 0, NULL,
		 &rcmd, &rlen, &rbuf);
    if (rcmd != CMD_SIZE || rlen != 4) {
      if (putPacket(CMD_RETRY, 0, NULL)
	  || getPacket(&rcmd, &rlen, &rbuf))
	resync();
    }
  } while (rcmd != CMD_SIZE || rlen != 4);

  romSize = rbuf[0] | (rbuf[1]<<8) | (rbuf[2]<<16) | (rbuf[3]<<24);

  for (currentPos = 0; currentPos < romSize;) {
    cbuf[0] = currentPos&0xff;
    cbuf[1] = (currentPos>>8)&0xff;
    cbuf[2] = (currentPos>>16)&0xff;
    cbuf[3] = (currentPos>>24)&0xff;

    do {
      putGetPacket(CMD_REQ, 4, cbuf,
		   &rcmd, &rlen, &rbuf);
      if (rcmd != CMD_DATA && rcmd != CMD_REPEAT) {
	if (putPacket(CMD_RETRY, 0, NULL)
	    || getPacket(&rcmd, &rlen, &rbuf))
	  resync();
      }
    } while (rcmd != CMD_DATA && rcmd != CMD_REPEAT);

    if (rcmd == CMD_REPEAT) {
      rlen -= 2;
      replen = rbuf[0] | (rbuf[1]<<8);
      for (i=0; i<replen; i+=rlen) {
	fwrite(rbuf+2, 1, rlen, rfile);
	currentPos += rlen;
      }
    }
    else {
      fwrite(rbuf, 1, rlen, rfile);
      currentPos += rlen;
    }
  }

  putGetPacket(CMD_EOT, 0, NULL,
	       &rcmd, &rlen, &rbuf);

  fclose(rfile);
  tcerr(lc.close());
  tcerr(lc.exit());
  ticable_exit();
  return 0;
}

/* Print out a packet */
void printPacket(int outgoing, word cmd, word length, byte *buf, word cksum)
{
  word i;

  fprintf(stderr,"%s: %04X[%04X] ", outgoing?"PC":"TI", cmd, length);

  for (i = 0; i < length && i < 8 ; i++) {
    fprintf(stderr, "%02X", buf[i]);
  }

  if (i < length)
    fprintf(stderr," ...");

  fprintf(stderr, " S=%04X\n", cksum);
}


/* Get a packet; buffer will be malloc'ed */
int getPacket(word *cmd, word *length, byte **buf)
{
  word i, sum;

  if (*buf) {
    free(*buf);
    *buf = NULL;
  }

  do {

    checksum = 0;

    if (getWord(cmd)) return 1;
    if (getWord(length)) return 1;

    if (*length > 0) {
      *buf = malloc(*length);
      if (*buf == NULL) {
	fprintf(stderr,"out of memory\n");
	exit(2);
      }
    }
    
    for (i = 0; i < *length; i++)
      if (getByte((*buf)+i)) {
	fprintf(stderr,"failed at %x\n",i);
	return 1;
      }
    
    sum = checksum;
    if (getWord(&i)) { fprintf(stderr, "failed before check\n"); return 1; }

    printPacket(0, *cmd, *length, *buf, i);

    if ((sum & 0xffff) != i)
      if (putPacket(CMD_RETRY, 0, NULL)) return 1;

  } while ((sum & 0xffff) != i);

  return 0;
}


/* Send a packet */
int putPacket(word cmd, word length, byte *buf)
{
  word i;

  checksum = 0;

  if (putWord(cmd)) return 1;
  if (putWord(length)) return 1;

  for (i = 0; i < length; i++)
    if (putByte(buf[i]))
      return 1;

  if (putWord(i = checksum)) return 1;

  printPacket(1, cmd, length, buf, i);

  return 0;
}


/* Send a packet and get response */
int putGetPacket(word scmd, word slen, byte *sbuf,
		 word *rcmd, word *rlen, byte **rbuf)
{
  int status;

  do {
    status = putPacket(scmd, slen, sbuf)
      || getPacket(rcmd, rlen, rbuf);
 
    if (status)
      resync();
  }
  while (status || *rcmd == CMD_RETRY);

  return 0;
}


/* Receive a byte */
int getByte(byte *b)
{
  if (lc.get(b)) return 1;
  checksum += *b;
  return 0;
}

/* Send a byte */
int putByte(byte b)
{
  if (lc.put(b)) return 1;
  checksum += b;
  return 0;
}

/* Receive a 16-bit word */
int getWord(word *w)
{
  byte a, b;
  if (getByte(&a)) return 1;
  if (getByte(&b)) return 1;
  *w = a | (b<<8);
  return 0;
}

/* Send a 16-bit word */
int putWord(word w)
{
  if (putByte(w&0xff)) return 1;
  if (putByte((w>>8)&0xff)) return 1;
  return 0;
}
