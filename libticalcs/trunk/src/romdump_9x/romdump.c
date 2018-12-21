//
//
// NOTE: this file is obsolete, it was superseded by the pure ASM version optimized from the output of the compiler on this file. It's only kept for reference.
//
//

/* Hey EMACS -*- linux-c -*- */

/*  RomDumper - an TI89/92/92+/V200PLT/Titanium ROM dumper
 *
 *  Copyright (c)      2002, Thomas Nussbaumer for the hardware detection code
 *  Copyright (c) 2004-2005, Romain Liévin for the TiLP and TiEmu projects
 *  Copyright (c) 2003-2006, Kevin Kofler for the hardware detection code, Fargo-II port and the USB code (taken from Backgammon)
 *  Copyright (c) 2003-2018, Lionel Debroux for the optimized hardware detection code, the optimized ROM dumper code
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


#define NULL ((void *)0)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

enum Attrs {A_REVERSE, A_NORMAL, A_XOR, A_SHADED, A_REPLACE, A_OR, A_AND, A_THICK1, A_SHADE_V, A_SHADE_H, A_SHADE_NS, A_SHADE_PS};
enum Fonts {F_4x6, F_6x8, F_8x10};
#define LCD_SIZE 3840

register void ** tios_table asm("%a5");

#ifdef FARGO

// #define __kb_globals tios__001b
// extern void __kb_globals;
// #define kbhit() (*(volatile short*)(&__kb_globals+0x1c))
#define kbhit() *((volatile short*)(tios_table[0x001B])+0x1C/2)
// #define sprintf tios__000f
// short sprintf (char *buffer, const char *format, ...);
#define sprintf (*(short (*) (char *, const char *, ...))(tios_table[0x000F]))
// #define DrawStr tios__0010
// void DrawStr (short x, short y, const char *str, short Attr);
#define DrawStr (*(void (*) (short, short, const char *, short))(tios_table[0x0010]))
// #define __ROM_base tios__0025
// extern void __attribute__((may_alias)) __ROM_base;
// #define ROM_base (&__ROM_base)
// ROM_base is defined later
// ROM_size is defined later
#define TI92_VERSION ((int)(ROM_size>>20))
// #define ERD_dialog tios__002F
// short ERD_dialog (short err_no, short prog_flag);
#define ERD_dialog (*(short (*) (short, short))(tios_table[0x002F]))
// #define OSLinkTxQueueInquire tios__0008
// unsigned short OSLinkTxQueueInquire (void);
#define OSLinkTxQueueInquire (*(unsigned short (*) (void))(tios_table[0x0008]))
// #define OSReadLinkBlock tios__000A
// unsigned short OSReadLinkBlock (char *buffer, unsigned short num);
#define OSReadLinkBlock (*(unsigned short (*) (char *, unsigned short))(tios_table[0x000A]))
// #define memset tios__0032
// void *memset (void *buffer, short c, unsigned long num);
#define memset (*(void * (*) (void *, short, unsigned long))(tios_table[0x0032]))
// #define __tios_globals tios__001C
// extern void __tios_globals;
// #define LCD_MEM (&__tios_globals)
#define LCD_MEM (tios_table[0x001C])
#define ClrScr() memset(LCD_MEM,0,LCD_SIZE)
// #define FontSetSys tios__0012
// unsigned char FontSetSys (short Font);
#define FontSetSys (*(unsigned char (*) (short))(tios_table[0x0012]))
// #define OSLinkReset tios__0006
// void OSLinkReset (void);
#define OSLinkReset (*(void (*) (void))(tios_table[0x0006]))

// The LIO functions in the TI-92 ROM are not exported by Fargo.

#else

#ifdef __TIGCC_ENV__
asm(".xdef _nostub\n"
".xdef _ti89ti\n");

#ifndef DUSB_DUMPER
asm(".xdef _ti89\n"
".xdef _ti92plus\n"
".xdef _v200\n");
#endif
#endif

#define kbhit (*(short (*)(void))(tios_table[0x0052]))
#define sprintf (*(short (*) (char *, const char *, ...))(tios_table[0x0053]))
#define DrawStr (*(void (*) (short, short, const char *, short))(tios_table[0x01A9]))
// ROM_base is defined later
// ROM_size is defined later
#define OSLinkTxQueueInquire (*(unsigned short (*) (void))(tios_table[0x0251]))
#define OSReadLinkBlock (*(unsigned short (*) (char *, short))(tios_table[0x024F]))
#define memset (*(void * (*) (void *, short, unsigned long))(tios_table[0x027C]))
#define memcpy (*(void * (*) (void *, const void *, unsigned long))(tios_table[0x026A]))
#define LCD_MEM ((void *)0x4C00)
#define ClrScr() memset(LCD_MEM,0,LCD_SIZE)
#define FontSetSys (*(unsigned char (*) (short))(tios_table[0x018F]))
#define OSLinkReset (*(void (*) (void))(tios_table[0x024C]))
#define LIO_SendData (*(short (* __attribute__((__stkparm__))) (const void *, unsigned long))(tios_table[0x005A]))
#define LIO_RecvData (*(short (* __attribute__((__stkparm__))) (void *, unsigned long, unsigned long))(tios_table[0x005B]))

#define XR_stringPtr_addr (unsigned char *)(tios_table[0x0293])
#define push_offset_array_addr (unsigned char *)(tios_table[0x03C4])
#define IsSupportedUSBAMS() (IsSupportedUSBAMS30x() || IsSupportedUSBAMS31x())
#define IsSupportedUSBAMS30x() (*(long long*) (XR_stringPtr_addr - 116) == 0x76021003261f4e75ll)
#define IsSupportedUSBAMS31x() (*(long long*) (push_offset_array_addr - 1112) == 0x76021003261f4e75ll)
#define DetectLinkInterface30x ((unsigned char(*) (void))(XR_stringPtr_addr - 158))
#define DetectReceiveInterface30x ((unsigned char(*) (short))(XR_stringPtr_addr - 108))
#define DetectLinkInterface31x ((unsigned char(*) (void))(push_offset_array_addr - 1154))
#define DetectReceiveInterface31x ((unsigned char(*) (short))(push_offset_array_addr - 1104))
#define DetectLinkInterface (IsSupportedUSBAMS31x() ? DetectLinkInterface31x : DetectLinkInterface30x)
#define DetectReceiveInterface (IsSupportedUSBAMS31x() ? DetectReceiveInterface31x : DetectReceiveInterface30x)
#define DetectUSB (*(short(**) (void))(DetectLinkInterface + 4))
#define USBCheckReceived (*(void(**) (void))(DetectReceiveInterface + 16))
#define USBCheckStatus (*(short(**) (void))(DetectReceiveInterface + 22))
#define USB_SendData ((short(* __attribute__((__stkparm__))) (const void *, unsigned long, unsigned long))(CheckStatus - 494))
#define USB_RecvData ((short(* __attribute__((__stkparm__))) (void *, unsigned long, unsigned long))(CheckStatus - 246))

#define USBLinkClose ((short(*) (void))(CheckStatus - 266))
#define USBLinkReset (*(void(**) (void))(DetectLinkInterface + 36))

#endif

#include "romdump.h"

register short __attribute__((__stkparm__)) (*SendData) (const void *src, unsigned long size
#ifdef DUSB_DUMPER
, unsigned long WaitDelay
#endif
) asm("%a3");
register short __attribute__((__stkparm__)) (*RecvData) (void *dest, unsigned long size, unsigned long WaitDelay) asm("%a4");

#ifdef __TIGCC_ENV__
asm(".globl call_main\n"
"call_main:\n"
"	jra _main");
#endif

// --- Packet Layer

#define BLK_SIZE (4096)

static inline __attribute__((always_inline)) uint16_t CheckSum(uint8_t* data, uint16_t len)
{
	uint16_t i;
	uint16_t sum = 0;

	for(i = 0; i < len; i++) {
		sum += *data++;
	}

	return sum;
}

// TODO macro PUSH_TIMEOUT_IF_NECESSARY: pea TIMEOUT.w if DUSB_DUMPER, otherwise nothing
// TODO macro ADJUST_SP_AFTER_SENDDATA: lea 12(%sp),%sp if DUSB_DUMPER, otherwise addq.l #8,%sp
// TODO macro ADJUST_SP_AFTER_RECVDATA: lea 12(%sp),%sp

int _SendPacket(uint8_t* buf asm("%a0"), uint16_t len asm("%d4"), uint16_t sum asm("%d5"), uint8_t* data asm("%a2"))
{
	// DONE for the ASM version: if the stack is organized in such a way that a6 points to buf, we can improve the code.
	short ret;

// | ret = SendPacket(buf, 4, TIMEOUT)
	// PUSH_TIMEOUT_IF_NECESSARY   | WaitDelay
	// pea      4.w                | size
	// pea      (%a6)              | buf
	// jsr      (%a3)
	// ADJUST_SP_AFTER_SENDDATA
	// tst.w    %d0
	// bne.s    __SendPacket_end   | bail out if nonzero

	// tst.w    %d3                | len == 0 ?
	// beq.s    __SendPacket_skip_send_len_and_compute_checksum

// | ret = SendPacket(data, len, TIMEOUT)
	// PUSH_TIMEOUT_IF_NECESSARY   | WaitDelay
	// move.w   %d3,-(%sp)         | size unsigned-extended to 32 bits
	// clr.w    -(%sp)             |
	// pea      (%a2)              | data
	// jsr      (%a3)
	// ADJUST_SP_AFTER_SENDDATA
	// tst.w    %d0
	// bne.s    __SendPacket_end   | bail out if nonzero

	// TODO the usual loop based on dbf.
	// subq.w   #1,%d3             | We don't need d3 anymore below, we'll restore its value later.
	// moveq    #0,%d1             | scratch register
// __SendPacket_loop_checksum:
	// move.b   (%a2)+,%d1         | sum += current byte from data. We're modifying a2 because we don't need it anymore after this loop, we'll restore its value later.
	// add.w    %d1,%d4            | sum
	// dbf      %d3,__SendPacket_loop_checksum

// __SendPacket_skip_send_len_and_compute_checksum:
	// rol.w    #8,%d4             | We don't need d3 anymore below, we'll restore its value later.
	// move.w   %d4,(%a6)          | Store to buf.

// | ret = SendPacket(buf, 2, TIMEOUT)
	// PUSH_TIMEOUT_IF_NECESSARY   | WaitDelay
	// pea      4.w                | size
	// pea      (%a6)              | tmp
	// jsr      (%a3)
	// ADJUST_SP_AFTER_SENDDATA

	ret = SendData(buf, 4
#ifdef DUSB_DUMPER
	, TIMEOUT
#endif
	);
	if (!ret) {
asm volatile("__mark_1_SendPacket:");
		// data
		ret = SendData(data, len
#ifdef DUSB_DUMPER
		, TIMEOUT
#endif
		);
		if (!ret) {
asm volatile("__mark_2_SendPacket:");
			// checksum
			//sum = CheckSum(buf, 4) + CheckSum(data, len);
			// We can even reuse buf...
			sum += CheckSum(data, len);
			sum = (sum << 8) | (sum >> 8);
			*(uint16_t *)buf = sum;

			// send
			ret = SendData(buf, 2
#ifdef DUSB_DUMPER
			, TIMEOUT
#endif
			);
		}
	}

asm volatile("__mark_3_SendPacket:");
	return ret;
// __SendPacket_end:
	// DONE for the ASM version: restore one more register to match the previous oversized save.
	// movem.l (%sp)+,%d2-%d4/%a2
}

static inline __attribute__((always_inline)) int SendPacket(uint8_t* buf asm("%a0"), uint16_t cmd asm("%d0"), uint16_t len asm("%d4"), uint8_t* data asm("%a2"))
{
	// We only pass constant arguments to SendPacket.
	// Also, we made buf aligned.
	// => we can write command and length on the caller side, letting the compiler reverse endianness while computing the 4 first bytes' checksum.
	*(uint32_t *)buf = (((uint32_t)((cmd << 8) | (cmd >> 8)) << 16) | ((len << 8) | (len >> 8)));
	return _SendPacket(buf,
	                   len,
	                   (cmd >> 8) + (cmd & 0xFF) + (len >> 8) + (len & 0xFF),
	                   data);
}

static void RecvPacket(uint8_t* buf, uint16_t* cmd, uint16_t* len)
{
	short ret;

asm volatile("__mark_begin_RecvPacket:");
	// default values
	*cmd = CMD_NONE;
	*len = 0;

	// any packet has always at least 4 bytes (cmd, len)
	ret = RecvData(buf, 4, TIMEOUT);
	if (!ret) {
asm volatile("__mark_1_RecvPacket:");
		uint16_t val;
		// TODO use either
		// move.b 1(%an),-(%sp); move.w (%sp)+,%dn; move.b (%an),%dn
		// or the more obvious and smaller
		// move.w (%an),%dn; rol.w #8,%dn
		// for aligned input data.
		// Read LE input data and convert to BE.
		val = *(uint16_t *)buf;
		val = (val << 8) | (val >> 8);
		*cmd = val;
		val = *((uint16_t *)buf + 1);
		val = (val << 8) | (val >> 8);
		*len = val;

		// the computer shouldn't be sending packets of excessive or odd size, but...
		if (val <= BLK_SIZE && !(val & 1)) {
asm volatile("__mark_2_RecvPacket:");
			// data part
			ret = RecvData(buf + 4, val, TIMEOUT);
			if (!ret) {
asm volatile("__mark_3_RecvPacket:");
				// checksum
				ret = RecvData(buf + 4 + val, 2, TIMEOUT);
				if (!ret) {
asm volatile("__mark_4_RecvPacket:");
					uint16_t sum = *(uint16_t *)(buf + 4 + val);
					sum = (sum << 8) | (sum >> 8);

					if (sum != CheckSum(buf, val + 4)) {
asm volatile("__mark_5_RecvPacket:");
						*cmd = CMD_NONE;
					}
				}
				// else do nothing.
			}
			// else do nothing.
		}
		// else do nothing.
	}
	// else do nothing.

asm volatile("__mark_end_RecvPacket:");
}

// --- Command Layer

static inline int Send_OK(uint8_t* buf)
{
asm volatile("__mark_begin_Send_OK:");
	int ret = SendPacket(buf, CMD_OK, 0, NULL);
asm volatile("__mark_end_Send_OK:");
	return ret;
}

static inline int Send_KO(uint8_t* buf)
{
asm volatile("__mark_begin_Send_KO:");
	int ret = SendPacket(buf, CMD_KO, 0, NULL);
asm volatile("__mark_end_Send_KO:");
	return ret;
}

static inline int Send_SIZE(uint8_t* buf, uint32_t size)
{
asm volatile("__mark_begin_Send_SIZE:");
	// TODO for the ASM version: make permanent 4-byte scratch space on stack ?
	// Nah, we can simply use buf here.
	uint32_t le_size = LE_BE(size);
	// For the 89/92+/V200/89T DBUS ROM dumper, the compiler made a stupefying optimization here: it knows that size = ROM_size,
	// which itself was computed as tios_table & 0xE00000. So it knows that only bits 16-23 in the register contain something.
	// After reversing the endianness, the nonzero bits have moved to bits 8-15.
	// So the compiler just generates a lsr.l #8, which is correct, even though it doesn't seem to be at first glance !
	// Heck, we could force that optimization into the Fargo dumper as well.
	*((uint32_t *)(buf + 4)) = le_size;

	int ret = SendPacket(buf, CMD_REQ_SIZE, 4, buf + 4);
asm volatile("__mark_end_Send_SIZE:");
	return ret;
}

static inline int Send_DATA(uint8_t* buf, uint16_t len, uint8_t* data)
{
asm volatile("__mark_begin_Send_DATA:");
	int ret = SendPacket(buf, CMD_DATA1, len, data);
asm volatile("__mark_end_Send_DATA:");
	return ret;
}

static inline int Send_RDATA(uint8_t* buf, uint16_t data)
{
asm volatile("__mark_begin_Send_RDATA:");
	// We can simply use buf here as well.
	*(uint16_t *)(buf + 4) = (BLK_SIZE << 8) | (BLK_SIZE >> 8);
	*(uint16_t *)(buf + 6) = (data << 8); // The 8 MSB of data are 0, given that it comes from a byte.
	/*buf[4] = LSB(BLK_SIZE);
	buf[5] = MSB(BLK_SIZE);
	buf[6] = LSB(data);
	buf[7] = MSB(data);*/

	int ret = SendPacket(buf, CMD_DATA2, 4, buf + 4);
asm volatile("__mark_end_Send_RDATA:");
	return ret;
}

static inline int Send_EXIT(uint8_t* buf)
{
asm volatile("__mark_begin_Send_EXIT:");
	int ret = SendPacket(buf, CMD_EXIT, 0, NULL);
asm volatile("__mark_end_Send_EXIT:");
	return ret;
}

static inline int Send_ERR(uint8_t* buf)
{
asm volatile("__mark_begin_Send_ERR:");
	int ret = SendPacket(buf, CMD_ERROR, 0, NULL);
asm volatile("__mark_end_Send_ERR:");
	return ret;
}

void _main(void)
{
	char str[30];
	__attribute__((aligned(2))) uint8_t buf[BLK_SIZE + 3*2];
#ifndef FARGO
	uint8_t lcd[LCD_SIZE];
#endif
#ifdef DUSB_DUMPER
	short (* __attribute__((__stkparm__)) LinkClose)(void);
	short (* __attribute__((__stkparm__)) CheckStatus)(void);
#endif
	register unsigned long ROM_size asm("%d6");
	register unsigned long ROM_base asm("%d7");

asm volatile("__mark_begin_initial_setup_1:");
#ifdef FARGO
	// Search for tios__* pointers table backwards from the end of the RAM: at first assume 256 KB of RAM, then 128 KB of RAM.
	// We know that the table contains ROM_base, which is always 0x400000.
	// This method might not work with older Fargo versions, but neither doesn't this program, which needs ERD_dialog and memset.
	// The compiler tries to generate clever code but the result looks weird, so I might have screwed up, let's use inline ASM with C operands instead...
	{
		asm volatile("\n"
	"lea     0x40000,%%a5\n"
	"moveq   #0x40,%%d2\n"    // 0x40000
	"swap    %%d2\n"
"L.loop0:\n"
	"movea.l %%a5,%%a0\n"
	"moveq   #50-1,%%d0\n"    // Loop at most 50 times, that's more than enough on released versions of Fargo.
"L.loop1:\n"
	"move.l  -(%%a0),%%d1\n"
	"cmp.l   %%d1,%%d2\n"
	"dbeq    %%d0,L.loop1\n"
	"beq.s   L.maybefound\n"
// Reduce the amount of RAM by 128 KB.
	"suba.l  #0x20000,%%a5\n"
	"move.l  %%a5,%%d1\n"
	"bne.s   L.loop0\n"
	"jra     L.end\n"         // If we come here, a5 is 0 and we have therefore failed to find the table.
"L.maybefound:\n"
	"lea     -7*4(%%a0),%%a1\n"
	"moveq   #1,%%d2\n"       // 0x00008000 = 0x00000001 followed by a rotation of the 16 LSB.
	"ror.w   #1,%%d2\n"
	"move.l  -(%%a1),%%d1\n"  // ST_flags
	"cmp.l   %%d1,%%d2\n"
	"bls.s   L.loop1\n"
	"move.l  -(%%a1),%%d1\n"  // kb_globals
	"cmp.l   %%d1,%%d2\n"
	"bls.s   L.loop1\n"
	"move.l  -(%%a1),%%d1\n"  // global
	"cmp.l   %%d1,%%d2\n"
	"bls.s   L.loop1\n"
// If we come here, the table looks good enough... rewind to the beginning of the table while storing to tios_table.
	"lea     -27*4(%%a1),%%a5\n"
	"" ::: "d0", "d1", "d2", "a0", "a1");
	}
	ROM_base = (unsigned long)(tios_table[0x0025]);
	ROM_size = ((49[(unsigned long *)ROM_base] + 0xFFFFFUL) & 0x00F00000UL);
#else
	tios_table = *((void ***)0xC8);
	ROM_base = (unsigned long)tios_table & 0xE00000;

#if defined(DUSB_DUMPER)
	ROM_size = 0x400000UL; // Although some hunks of the boot code support such a thing, sadly, no known real-world 89T has more than 4 MB of Flash memory.
#else
	ROM_size = (ROM_base == 0x800000UL) || ((ROM_base == 0x200000UL) && ((unsigned char *)(tios_table[0x002F]))[2] > 200) ? 0x400000UL : 0x200000UL;
#endif

	memcpy(lcd, LCD_MEM, LCD_SIZE);
#endif
asm volatile("__mark_end_initial_setup_1:");

	ClrScr();
	FontSetSys(F_8x10);

	DrawStr(0, 0, "RomDumper v" VERSION, A_NORMAL);

#ifdef FARGO
	sprintf(str, "Type: TI-92 %d", TI92_VERSION);
#else
	{
		long hw_version;
		asm volatile("\n"
	"movea.l  %1,%%a0\n"
	"movea.l  260(%%a0),%%a1\n"  // get pointer to the hardware param block
	"adda.l   #0x10000,%%a0\n"
	"cmpa.l   %%a0,%%a1\n"       // check if the HW parameter block is near enough
	"bcc.s    L.is_hw1\n"        // if it is too far, it is HW1
	"cmpi.w   #22,(%%a1)\n"      // check if the parameter block contains HW ver
	"bls.s    L.is_hw1\n"        // if it is too small, it is HW1
"\n"
// Check for VTI (trick suggested by Julien Muchembled)
	"trap     #12\n"             // enter supervisor mode. returns old SR in d0.w
	"move.w   #0x3000,%%sr\n"    // set a non-existing flag in SR (but keep s-flag)
	"move.w   %%sr,%%d1\n"       // get SR content and check for non-existing flag
	"move.w   %%d0,%%sr\n"       // restore old SR content
	"lsl.w    #3,%%d1\n"         // the non-existing 0x1000 can only be set on VTI
	"bpl.s    L.not_vti\n"       // flag not set -> no VTI
"\n"
// VTI detected -> treat as HW1
// Fall through...
"\n"
"L.is_hw1:\n"
	// HW1 detected
	"moveq    #1,%0\n"           // set d0 to 1 (HW1)
	"bra.s    L.hw_end\n"
"\n"
"L.not_vti:\n"
	// Real calculator detected, so read the HW version from the HW parm block
	"move.l   22(%%a1),%0\n"     // get the hardware version
"L.hw_end:\n" : "=d" (hw_version) : "g" (ROM_base) : "d0", "d1", "a0", "a1");

		sprintf(str, "Type: HW%d", (short)hw_version);
	}

#endif
	DrawStr(0, 20, str, A_NORMAL);

	sprintf(str, "ROM base: 0x%lx", (uint32_t)ROM_base);
	DrawStr(0, 40, str, A_NORMAL);

	DrawStr(0, 80, "by The TiLP Team", A_NORMAL);

asm volatile("__mark_begin_initial_setup_2:");
#ifdef FARGO
	// Ugly hack to find LIO_SendData and LIO_RecvData...
	SendData=(void*)ERD_dialog;
	while (*(void**)SendData!=OSLinkTxQueueInquire) SendData+=2;
	RecvData=(void*)SendData;
	SendData-=28;
	while (*(void**)RecvData!=OSReadLinkBlock) RecvData+=2;
	RecvData-=150;
#elif defined(DUSB_DUMPER)
	CheckStatus=USBCheckStatus;
	SendData=USB_SendData;
	RecvData=USB_RecvData;
	LinkClose=USBLinkClose;
#else
	SendData=LIO_SendData;
	RecvData=LIO_RecvData;
#endif
asm volatile("__mark_end_initial_setup_2:");

asm volatile("__mark_before_loop:");
	for (;;) {
begin:
		uint16_t cmd, len;
		unsigned int i;
		// wait for command
		RecvPacket(buf, &cmd, &len);

		// or keypress
		if (kbhit()) {
			break;
		}

		// and process
asm volatile("__mark_begin_switch:");
		switch(cmd) {
			case CMD_IS_READY: 
				Send_OK(buf);
			break;
			case CMD_EXIT: 
				Send_EXIT(buf);
				goto end;
			break;
			case CMD_REQ_SIZE: 
				Send_SIZE(buf, ROM_size);

				sprintf(str, "Size: %lu KB", ROM_size >> 10);
				DrawStr(0, 60, str, A_REPLACE);
			break;
			case CMD_REQ_BLOCK:
			{
asm volatile("__mark_begin_cmd_req_block:");
				register uint32_t addr asm("%d3");
				register uint8_t * ptr asm("%a0");
				asm volatile("\n"
				"move.l (%1),%0\n"
				"rol.w #8,%0\n"
				"swap %0\n"
				"rol.w #8,%0\n"
				"" : "=d" (addr) : "g" (buf + 4) : "cc");

				if (addr > ROM_size) {
					Send_ERR(buf);
					goto begin;
				}

				//ptr = (uint8_t *)((unsigned long)tios_table & 0xE00000) + addr;

#if 0
				if(addr >= 0x10000 && addr < 0x12000) {	// read protected (certificate)
					addr = 0;
				}
				if(addr >= 0x18000 && addr < 0x1A000) {	// read protected (certificate)
					addr = 0;
				}
#endif

				sprintf(str, "Done: %lu/%luKB     ", addr >> 10, ROM_size >> 10);
				DrawStr(0, 60, str, A_REPLACE);

				ptr = (uint8_t *)ROM_base + addr;
				uint8_t val1 = ptr[0];
				// Check for filled blocks (like 0xff)
				for (i = 0; i < BLK_SIZE; i++) {
					if (*ptr++ != val1) {
						break;
					}
				}

				if (i == BLK_SIZE) {
					Send_RDATA(buf, val1);
				}
				else {
					Send_DATA(buf, BLK_SIZE, (uint8_t *)ROM_base + addr);
				}
asm volatile("__mark_end_cmd_req_block:");
			}
			break;
			case CMD_NONE:
			break;
			default:
			break;
		}
	}
end:

asm volatile("__mark_after_loop:");
#ifdef DUSB_DUMPER
	LinkClose();
#endif

#ifndef FARGO
	memcpy(LCD_MEM, lcd, LCD_SIZE);
#else
	asm volatile("L.end:");
#endif
}

#ifndef __TIGCC_ENV__
void main(void)
{
	// Global register variables are not saved/restored by standard toolchains...
	asm volatile("movem.l %a3-%a5,-(%sp)");
	_main();
	asm volatile("movem.l (%sp)+,%a3-%a5");
}
#endif
