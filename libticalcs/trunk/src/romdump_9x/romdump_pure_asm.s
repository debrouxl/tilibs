|  RomDumper - an TI89/92/92+/V200PLT/89T ROM dumper
|
|  Copyright (c)      2002, Thomas Nussbaumer for the hardware detection code
|  Copyright (c) 2004-2005, Romain Liévin for the TiLP and TiEmu projects
|  Copyright (c) 2003-2006, Kevin Kofler for the hardware detection code, Fargo-II port and the USB code (taken from Backgammon)
|  Copyright (c)      2009, Martial Demolins for the optimized SAVE_SCREEN code (used in an inlined fashion here)
|  Copyright (c) 2003-2021, Lionel Debroux for the optimized hardware detection code, the optimized ROM dumper code and the subsequent, compiler-assisted conversion to pure ASM.
|
|  This program is free software; you can redistribute it and/or modify
|  it under the terms of the GNU General Public License as published by
|  the Free Software Foundation; either version 2 of the License, or
|  (at your option) any later version.
|
|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.
|
|  You should have received a copy of the GNU General Public License
|  along with this program; if not, write to the Free Software Foundation,
|  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

| General defines
.set A_NORMAL, 1
.set A_REPLACE, 4
.set F_8x10, 2

.set STR_SIZE, 30
.set BLK_SIZE, 4096
.set BUF_SIZE, BLK_SIZE + 3 * 2
.set LCD_SIZE, 3840

.set TIMEOUT, 2000

.set CMD_IS_READY, 0xAA55
.set CMD_KO, 0x0000
.set CMD_OK, 0x0001
.set CMD_EXIT, 0x0002
.set CMD_REQ_SIZE, 0x0003
.set CMD_ERROR, 0x0004 | unused
.set CMD_REQ_BLOCK, 0x0005
.set CMD_DATA1, 0x0006
.set CMD_DATA2, 0x0007
.set CMD_NONE, 0xffff

| Stack-related defines
.ifdef FARGO
.set STACK_OFFSET_ABOVE_A6, BUF_SIZE
.set STACK_RESERVED_SPACE, STR_SIZE + STACK_OFFSET_ABOVE_A6
.else
| Possibly +4 for DUSB dumper, to store the address of LinkClose ?
.set STACK_OFFSET_ABOVE_A6, BUF_SIZE | plus the LCD_SIZE bytes implicitly reserved by saving the screen onto the stack
.set STACK_RESERVED_SPACE, STR_SIZE + STACK_OFFSET_ABOVE_A6
.set LCD_MEM, 0x4C00
.endif

| ROM_CALLs
.ifdef FARGO
.set DrawStr, 0x0010 * 4
.set ERD_dialog, 0x002F * 4
.set FontSetSys, 0x0012 * 4
.set kb_globals, 0x001B * 4
.set LCD_MEM, 0x001C * 4 | actually tios_globals, but since these globals effectively start at LCD_MEM, and we use only LCD_MEM...
.set OSLinkReset, 0x0006 * 4
.set OSLinkTxQueueInquire, 0x0008 * 4
.set OSReadLinkBlock, 0x000A * 4
.set sprintf, 0x000F * 4
.else
.set DrawStr, 0x01A9 * 4
.set FontSetSys, 0x018F * 4
.set kbhit, 0x0052 * 4
.set LCD_MEM, 0x4C00
.set LIO_SendData, 0x005A * 4
.set LIO_RecvData, 0x005B * 4
.set OSLinkReset, 0x024C * 4
.set OSLinkTxQueueInquire, 0x0251 * 4
.set OSReadLinkBlock, 0x024F * 4
.set push_offset_array, 0x03C4 * 4
.set ScrRect, 0x002F * 4
.set sprintf, 0x0053 * 4
.set XR_stringPtr, 0x0293 * 4
.endif

| Macros
.macro PUSH_TIMEOUT_IF_NECESSARY
.ifdef DUSB_DUMPER
	pea      TIMEOUT.w
.endif
.endm

.macro ADJUST_SP_AFTER_SENDDATA
.ifdef DUSB_DUMPER
	lea      12(%sp),%sp
.else
	addq.l   #8,%sp
.endif
.endm

.macro ADJUST_SP_AFTER_RECVDATA
	lea      12(%sp),%sp
.endm

| *(uint32_t *)buf = (((uint32_t)((cmd << 8) | (cmd >> 8)) << 16) | ((len << 8) | (len >> 8)));
.macro COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN dest, cmd, len
	move.l   #(((((\cmd << 8) + (\cmd >> 8)) & 0xFFFF) << 16) + (((\len << 8) + (\len >> 8)) & 0xFFFF)), (\dest)
.endm

.macro COMPUTE_CHECKSUM dest, cmd, len
	moveq    #((\cmd >> 8) + (\cmd & 0xFF) + (\len >> 8) + (\len & 0xFF)),\dest
.endm


	.even
	.text

.ifdef FARGO
	.xdef _fargo

| Header for environments without special TI-68k format handling.
.ifndef TIGCC_ENV

| Size bytes
	.byte (((__end_of_program_data - __beginning_of_program_data) >> 8) & 0xFF)
	.byte ((__end_of_program_data - __beginning_of_program_data) & 0xFF)

	.global __beginning_of_program_data
__beginning_of_program_data:
__fargo_program_header:
__fargo_program_signature:
	.word 0x0032
	.ascii "EXE APPL"
__fargo_reloc_count_pos:
	.word 0
__fargo_reloc_table_pos:
	.word __fargo_relocs - __fargo_program_signature + 2
__fargo_bss_table_pos:
	.word __fargo_bss_refs - __fargo_program_signature + 2
__fargo_import_table_pos:
	.word __fargo_libs - __fargo_program_signature + 2
__fargo_export_table_pos:
	.word __fargo_program_export_table - __fargo_program_signature + 2
__fargo_comment_pos:
	.word 0
__fargo_flags:
	.word 1
__fargo_program_export_table:
	.word 1
	.word _main - __fargo_program_signature + 2
.endif

.else | FARGO

| Control symbols for the linker
.ifdef TIGCC_ENV
	.xdef _nostub
	.xdef _ti89ti

.ifndef DUSB_DUMPER
	.xdef _ti89
	.xdef _ti92plus
	.xdef _v200
.endif

.else

| Size bytes
	.byte (((__end_of_program_data - __beginning_of_program_data) >> 8) & 0xFF)
	.byte ((__end_of_program_data - __beginning_of_program_data) & 0xFF)

	.global __beginning_of_program_data
__beginning_of_program_data:

.endif | TIGCC_ENV

.endif | FARGO

	.global _main
_main:
	movem.l  %d3-%d7/%a2-%a6,-(%sp)

__mark_begin_initial_setup_1:

.ifdef FARGO

| Search for tios__* pointers table backwards from the end of the RAM: at first assume 256 KB of RAM, then 128 KB of RAM.
| We know that the table contains ROM_base, which is either 0x400000 or 0x200000, and can be determined from the vectors.
| This method might not work with older Fargo versions, but neither does this program, which needs ERD_dialog and memset.

	lea      0x40000,%a5
	move.l   0x4.w,%d7              | Load initial PC
	andi.l   #0xE00000,%d7

__fargo_initial_setup_1_loop0_begin:
	movea.l  %a5,%a0
	moveq    #50-1,%d0              | 50 iterations are enough

__fargo_initial_setup_1_loop1:
	move.l   -(%a0),%d1
	cmp.l    %d1,%d7
	dbeq     %d0,__fargo_initial_setup_1_loop1

	beq.s   __fargo_initial_setup_1_maybefound
| Try again for calculators with 128 KB of RAM.
	suba.l   #0x20000,%a5
	move.l   %a5,%d1
	bne.s    __fargo_initial_setup_1_loop0_begin

| If we reach this instruction, a5 is 0 and we have therefore failed to find the table.
| Simply get out of here, as we don't even know where to find a function which would let us print an error message.
	bra.w    __restore_regs_and_return

__fargo_initial_setup_1_maybefound:
	lea      -7*4(%a0),%a1
| In order to confirm that we found the appropriate entry, look for the pointers to RAM variables, located below 0x8000, contained in Fargo's table.
	moveq    #1,%d2                 | 0x00008000 = 0x00000001 followed by a rotation of the 16 LSB.
	ror.w    #1,%d2
	move.l   -(%a1),%d1             | ST_flags
	cmp.l    %d1,%d2
	bls.s    __fargo_initial_setup_1_loop1
	move.l   -(%a1),%d1             | global
	cmp.l    %d1,%d2
	bls.s    __fargo_initial_setup_1_loop1
	move.l   -(%a1),%d1             | kb_globals
	cmp.l    %d1,%d2
	bls.s    __fargo_initial_setup_1_loop1

| If we come here, the table looks good enough... rewind to the beginning of the table while storing to tios_table.
	lea      -27*4(%a1),%a5

	move.l   %d7,%a0                | Get ROM size: ((49[(unsigned long *)ROM_base] + 0xFFFFFUL) & 0x00F00000UL)
	move.l   0xC4(%a0),%d6
	addi.l   #0x0FFFFF,%d6
	andi.l   #0xF00000,%d6

.else

	move.l   0xC8.w,%a5             | jump table
	move.l   %a5,%d7                | Compute ROM_base
	andi.l   #0xE00000,%d7          | ROM_base = __jmp_tbl & 0xE00000

	moveq    #0x40,%d6              | Preload ROM_size >> 16 = 0x40

| Although some hunks of the boot code support such a thing, sadly, no known real-world 89T has more than 4 MB of Flash memory...
.ifndef DUSB_DUMPER

	btst.l   #23,%d7                | ROM_base == 0x800000 ? 89T, so we're done : not yet
	bne.s    __end_rom_size_determination
| No, we don't have a 89T.
	btst.l   #22,%d7                | ROM_base == 0x400000 ? 92+, so we're done : not yet
	bne.s    __rom_size_is_2M
| We have either a V200 or a 89.
	move.l   ScrRect(%a5),%a0
	cmpi.b   #-56,2(%a0)            | 200
	bhi.s    __end_rom_size_determination
__rom_size_is_2M:
	lsr.w    #1,%d6                 | Set ROM_size >> 16 = 0x20
__end_rom_size_determination:
.endif | DUSB_DUMPER

	swap    %d6                     | ROM_size = 0x200000 or ROM_size = 0x400000.

.endif | FARGO


| Save (+ clear, for non-Fargo) the screen
| Execute the loop LCD_SIZE/4 times (4 bytes are transferred at a time).
	move.w   #(LCD_SIZE/4)-1,%d0
.ifdef FARGO
	move.l   LCD_MEM(%a5),%a0
.else
	lea      LCD_MEM.w,%a0
.endif
__save_clear_screen_loop:
.ifndef FARGO
	move.l   (%a0),-(%sp)
.endif
	clr.l    (%a0)+
	dbf      %d0,__save_clear_screen_loop


	lea      -STACK_OFFSET_ABOVE_A6(%sp),%a6
	lea      -STACK_RESERVED_SPACE(%sp),%sp

__mark_end_initial_setup_1:

| Register allocation at this point:
| d6 = ROM_size
| d7 = ROM_base
| a5 = tios_table / __jmp_tbl
| a6 = buf
|
| Stack usage:
|
| * 92 Fargo version:
| +----------------------+----------------------+-----------------+
| | str (STR_SIZE bytes) | buf (BUF_SIZE bytes) | saved registers |
| +----------------------+----------------------+-----------------+
| ^                      ^                                        ^
| sp                     a6                              initial sp
|
| * 89/92+/V200/89T versions, whether DUSB_DUMPER or not:
| +----------------------+----------------------+-------------------------------+-----------------+
| | str (STR_SIZE bytes) | buf (BUF_SIZE bytes) | saved screen (LCD_SIZE bytes) | saved registers |
| +----------------------+----------------------+-------------------------------+-----------------+
| ^                      ^                                                                        ^
| sp                     a6                                                              initial sp


| Start printing stuff on the screen.
| FontSetSys(F_8x10); 2 bytes on the stack
	move.w   #F_8x10,-(%sp)
	move.l   FontSetSys(%a5),%a0
	jsr      (%a0)
| DrawStr(0, 0, "RomDumper " VERSION, A_NORMAL); 10 - 2 bytes on the stack
	move.w   #1,(%sp)
	pea      __str_ROMDumper_VERSION(%pc)
	clr.l    -(%sp)
	move.l   DrawStr(%a5),%a0
	jsr      (%a0)

| Determine the hardware revision.
.ifdef FARGO
	move.l   %d6,%d0                | #define TI92_VERSION ((int)(ROM_size>>20))
	swap     %d0
	lsr.w    #4,%d0
| d0: TI92_VERSION
	move.w   %d0,(%sp)
.else
	movea.l  %d7,%a0                | ROM_base
	movea.l  260(%a0),%a1           | get pointer to the hardware param block
	adda.l   #0x10000,%a0
	cmpa.l   %a0,%a1                | check if the HW parameter block is near enough
	bcc.s    __hwdetect_hw1         | if it is too far, it is HW1
	cmpi.w   #22,(%a1)              | check if the parameter block contains HW ver
	bls.s    __hwdetect_hw1         | if it is too small, it is HW1
| Check for VTI (trick suggested by Julien Muchembled)
	trap     #12                    | enter supervisor mode. returns old (%sr) in %d0.w
	move.w   #0x3000,%sr            | set a nonexistent flag in %sr (but keep s-flag)
	move.w   %sr,%d1                | get %sr content and check for nonexistent flag
	move.w   %d0,%sr                | restore old %sr content
	lsl.w    #3,%d1                 | the non-existing 0x1000 can only be set on VTI
	bpl.s    __hwdetect_not_vti     | flag not set => not VTI
| VTI detected -> treat as HW1
| Fall through
__hwdetect_hw1:
| HW1 detected
	moveq    #1,%d0
	bra.s    __hwdetect_end
__hwdetect_not_vti:
| Real calculator detected, so read the HW version from the HW parm block
	move.l   22(%a1),%d0
__hwdetect_end:
| d0: hw_version
	move.w   %d0,(%sp)
.endif

| Initialization: CSE on function addresses and str address
	move.l   sprintf(%a5),%a4
	move.l   DrawStr(%a5),%a3
	lea      -STR_SIZE(%a6),%a2

| sprintf(str, "Type: TI-92 %d" or "Type: HW%d", TI92_VERSION or hw_version); 10 - 2 bytes on the stack
	pea      __str_Type(%pc)
	pea      (%a2)
	jsr      (%a4)
| DrawStr(0, 20, str, A_NORMAL); 10 - 2 bytes on the stack
	move.w   #A_NORMAL,(%sp)
	pea      (%a2)
	pea      20.w
	jsr      (%a3)

| sprintf(str, "ROM base: 0x%lx", (uint32_t)ROM_base); 12 - 4 bytes on the stack
	move.l   %d7,(%sp)
	pea      __str_ROMbase(%pc)
	pea      (%a2)
	jsr      (%a4)

| DrawStr(0, 40, str, A_NORMAL); 10 - 2 bytes on the stack
	move.w   #A_NORMAL,(%sp)
	pea      (%a2)
	pea      40.w
	jsr      (%a3)

| DrawStr(0, 80, "by The TiLP Team", A_NORMAL); 10 - 2 bytes on the stack
	move.w   #A_NORMAL,(%sp)
	pea      __str_Credits(%pc)
	pea      80.w
	jsr      (%a3)


| Now clean up the stack, we've pushed a whole lot of stuff on it :)
	lea      (2 + (10 - 2) + (10 - 2) + (10 - 2) + (12 - 4) + (10 - 2) + (10 - 2)) (%sp),%sp


__mark_begin_initial_setup_2:
.ifdef FARGO
| Ugly hack to find LIO_SendData and LIO_RecvData...
	move.l   ERD_dialog(%a5),%a3    | SendData = (void *)ERD_dialog;
	move.l   OSLinkTxQueueInquire(%a5),%d0

| While...
	bra.s    __fargo_initial_setup_2_loop1_entry
__fargo_initial_setup_2_loop1_begin:
	addq.l   #2,%a3                 | SendData += 2;
__fargo_initial_setup_2_loop1_entry:
	cmp.l    (%a3),%d0              | *(void**)SendData == OSLinkTxQueueInquire ?
	bne.s    __fargo_initial_setup_2_loop1_begin
| Found OSLinkTxQueueInquire
| RecvData=(void*)SendData;
	move.l   %a3,%a4                | RecvData = (void*)SendData;
	lea      -28(%a4),%a3           | SendData -= 28;
	move.l   OSReadLinkBlock(%a5),%d0

| While...
	bra.s    __fargo_initial_setup_2_loop2_entry
__fargo_initial_setup_2_loop2_begin:
	addq.l   #2,%a4                 | RecvData += 2;
__fargo_initial_setup_2_loop2_entry:
	cmp.l    (%a4),%d0              | *(void**)RecvData == OSReadLinkBlock ?
	bne.s    __fargo_initial_setup_2_loop2_begin
| Found OSReadLinkBlock
	lea      -150(%a4),%a4          | RecvData -= 150;

.else

.ifdef DUSB_DUMPER
| #define IsSupportedUSBAMS() (IsSupportedUSBAMS30x() || IsSupportedUSBAMS31x())
| #define IsSupportedUSBAMS30x() (*(long long*) (XR_stringPtr_addr - 116) == 0x76021003261f4e75ll)
| #define IsSupportedUSBAMS31x() (*(long long*) (push_offset_array_addr - 1112) == 0x76021003261f4e75ll)
| #define DetectLinkInterface30x ((unsigned char(*) (void))(XR_stringPtr_addr - 158))
| #define DetectReceiveInterface30x ((unsigned char(*) (short))(XR_stringPtr_addr - 108))
| #define DetectLinkInterface31x ((unsigned char(*) (void))(push_offset_array_addr - 1154))
| #define DetectReceiveInterface31x ((unsigned char(*) (short))(push_offset_array_addr - 1104))
| #define DetectLinkInterface (IsSupportedUSBAMS31x() ? DetectLinkInterface31x : DetectLinkInterface30x)
| #define DetectReceiveInterface (IsSupportedUSBAMS31x() ? DetectReceiveInterface31x : DetectReceiveInterface30x)
| #define DetectUSB (*(short(**) (void))(DetectLinkInterface + 4))
| #define USBCheckReceived (*(void(**) (void))(DetectReceiveInterface + 16))
| #define USBCheckStatus (*(short(**) (void))(DetectReceiveInterface + 22))
| #define USB_SendData ((short(* __attribute__((__stkparm__))) (const void *, unsigned long, unsigned long))(CheckStatus - 494))
| #define USB_RecvData ((short(* __attribute__((__stkparm__))) (void *, unsigned long, unsigned long))(CheckStatus - 246))
| 
| #define USBLinkClose ((short(*) (void))(CheckStatus - 266))
| #define USBLinkReset (*(void(**) (void))(DetectLinkInterface + 36))

| CheckStatus = USBCheckStatus;
| SendData = USB_SendData;
| RecvData = USB_RecvData;
| LinkClose = USBLinkClose;

	move.l   push_offset_array(%a5),%a0
	move.l   -1112(%a0),%d0
	move.l   -1108(%a0),%d1
	move.l   #0x76021003,%d2
	move.l   #0x261f4e75,%d3
	sub.l    %d3,%d1
	subx.l   %d2,%d0
	bne.s    .L10
	lea      -1082(%a0),%a0
	bra.s    .L12
.L10:
	move.w   #-86,%a0
	add.l    XR_stringPtr(%a5),%a0
.L12:
	move.l   (%a0),%a2              | CheckStatus
	lea      -494(%a2),%a3          | SendData = CheckStatus - 494
	lea      -246(%a2),%a4          | RecvData = CheckStatus - 246
| Nah, don't have enough registers in the loop when doing that. We'll just use a jsr (266 - 246)(%a4) at the end.
	|lea      -266(%a2),%a2          | USBLinkClose = CheckStatus - 266
	|move.l   %a2,%d5

.else
| It's so refreshingly easy when one can just use ROM_CALLs...
	movea.l  LIO_SendData(%a5),%a3  | SendData
	movea.l  LIO_RecvData(%a5),%a4  | RecvData

.endif | DUSB_DUMPER

.endif | FARGO

__mark_end_initial_setup_2:

__mark_before_loop:
	moveq    #0,%d4                 | Clear the upper part of len once and for all, so that we can push it as long when necessary.

| Register allocation:
| (d0-d2: call-clobbered registers, scratch)
| (d3: call-preserved scratch register)
| d4 = len after inline RecvPacket and before _SendPacket
| d5 = cmd after inline RecvPacket, checksum before _SendPacket
| d6 = ROM_size
| d7 = ROM_base
| (a0-a1: call-clobbered registers, scratch)
| a2 = pointer to additional data
| a3 = SendData
| a4 = RecvData
| a5 = tios_table / __jmp_tbl
| a6 = buf
|
| Stack allocation: same as before.


__main_loop_start:
__mark_begin_RecvPacket:
| Header part: any packet has always at least 4 bytes (cmd, len)
| RecvData(buf, 4, TIMEOUT);
	pea      TIMEOUT.w
	pea      4.w
	pea      (%a6)
	jsr      (%a4)
	ADJUST_SP_AFTER_RECVDATA
	tst.w    %d0
	bne.s    __RecvPacket_set_CMD_NONE
__mark_1_RecvPacket:
	move.w   (%a6),%d5              | received cmd
	ror.w    #8,%d5                 | cmd after swapping endianness
	move.w   2(%a6),%d4             | received len
	ror.w    #8,%d4                 | len after swapping endianness

| The computer shouldn't be sending packets of excessive or odd size, but...
	cmpi.w   #4096,%d4
	bhi.s    __RecvPacket_end
	btst     #0,%d4
	bne.s    __RecvPacket_end
__mark_2_RecvPacket:
| Data part
| RecvData(buf + 4, len, TIMEOUT);
	pea      TIMEOUT.w
	move.l   %d4,-(%sp)
	pea      4(%a6)
	jsr      (%a4)
	ADJUST_SP_AFTER_RECVDATA
	tst.w    %d0
	bne.s    __RecvPacket_end
__mark_3_RecvPacket:
| Checksum part
| RecvData(buf + 4 + val, 2, TIMEOUT);
	pea      TIMEOUT.w
	pea      2.w
	pea      4(%a6,%d4.w)
	jsr      (%a4)
	ADJUST_SP_AFTER_RECVDATA
	tst.w    %d0
	bne.s    __RecvPacket_end
__mark_4_RecvPacket:
	move.w   4(%a6,%d4.w),%d2
	ror.w    #8,%d2                 | checksum in packet
	move.w   %d4,%d3                | len temp
	addq.w   #4-1,%d3               | add 4 for checksum computation on header + data, subtract 1 for dbf.
	move.l   %a6,%a0
	moveq    #0,%d0                 | current byte
	moveq    #0,%d1                 | partial checksum in the making.
__RecvPacket_loop_checksum:
	move.b   (%a0)+,%d0
	add.w    %d0,%d1                | add current byte to partial checksum in the making.
	dbf      %d3,__RecvPacket_loop_checksum
	cmp.w    %d1,%d2                | checksum in packet == computed checksum ?
	beq.s    __RecvPacket_end
__mark_5_RecvPacket:
__RecvPacket_set_CMD_NONE:
	moveq    #-1,%d5                | cmd = CMD_NONE;
__RecvPacket_end:
__mark_end_RecvPacket:

.ifdef FARGO
| #define kbhit() *((volatile short*)(tios_table[0x001B])+0x1C/2)
	lea      28.w,%a0
	add.l    kb_globals(%a5),%a0
	move.w   (%a0),%d0
.else
| kbhit()
	move.l   kbhit(%a5),%a0
	jsr      (%a0)
	tst.w    %d0
.endif
	jbne     __main_loop_end
__mark_begin_switch:
| TODO use a subq sequence here, destroying d5 in the process.
/*
	cmpi.w   #CMD_EXIT,%d5
	beq.s    __do_Send_EXIT
	cmpi.w   #CMD_REQ_SIZE,%d5
	jbeq     __do_Send_SIZE
	cmpi.w   #CMD_REQ_BLOCK,%d5
	jbeq     __do_Handle_REQ_BLOCK
*/
	subq.w   #CMD_EXIT,%d5
	beq.s    __do_Send_EXIT
	subq.w   #(CMD_REQ_SIZE-CMD_EXIT),%d5
	jbeq     __do_Send_SIZE
	subq.w   #(CMD_REQ_BLOCK-CMD_REQ_SIZE),%d5
	jbeq     __do_Handle_REQ_BLOCK
	cmpi.w   #(CMD_IS_READY-CMD_REQ_BLOCK),%d5
	jbne     __main_loop_start
__mark_begin_Send_OK:
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_OK, 0
	| No need to set a2 in this path, since we aren't going to use it because len == 0.
	|suba.l   %a2,%a2
	COMPUTE_CHECKSUM %d5, CMD_OK, 0
	moveq    #0,%d4                 | len
	bsr.s    _SendPacket
__mark_end_Send_OK:
	bra.w    __main_loop_start
__do_Send_EXIT:
__mark_begin_Send_EXIT:
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_EXIT, 0
	| No need to set a2 in this path, since we aren't going to use it because len == 0.
	|suba.l   %a2,%a2
	COMPUTE_CHECKSUM %d5, CMD_EXIT, 0
	moveq    #0,%d4                 | len
	bsr.s    _SendPacket
__mark_end_Send_EXIT:
	bra.w    __main_loop_end


| _SendPacket was moved here, in order to be closer to the call sites.
_SendPacket:
	movem.l  %d4-%d5/%a2,-(%sp)

| SendData(buf, 4[, TIMEOUT]);
	PUSH_TIMEOUT_IF_NECESSARY
	pea      4.w
	pea      (%a6)
	jsr      (%a3)
	ADJUST_SP_AFTER_SENDDATA
	tst.w    %d0
	bne.s    __SendPacket_End

	tst.w    %d4                | len == 0 ?
	beq.s    __SendPacket_skip_send_len_and_compute_checksum
__mark_1_SendPacket:
	PUSH_TIMEOUT_IF_NECESSARY
	move.l   %d4,-(%sp)
	pea      (%a2)
	jsr      (%a3)
	ADJUST_SP_AFTER_SENDDATA
	tst.w    %d0
	bne.s    __SendPacket_End
__mark_2_SendPacket:

	subq.w   #1,%d4             | We don't need d4 anymore below, we'll restore its value later.
	moveq    #0,%d1             | scratch register
__SendPacket_loop_checksum:
	move.b   (%a2)+,%d1         | sum += current byte from data. We're modifying a2 because we don't need it anymore after this loop, we'll restore its value later.
	add.w    %d1,%d5            | sum
	dbf      %d4,__SendPacket_loop_checksum

__SendPacket_skip_send_len_and_compute_checksum:
	rol.w    #8,%d5             | reverse endianness
	move.w   %d5,(%a6)          | *(uint16_t *)buf = sum;
	PUSH_TIMEOUT_IF_NECESSARY
	pea      2.w
	pea      (%a6)
	jsr      (%a3)
	ADJUST_SP_AFTER_SENDDATA
__SendPacket_End:
__mark_3_SendPacket:
	movem.l  (%sp)+,%d4-%d5/%a2
	rts


__do_Send_SIZE:
__mark_begin_Send_SIZE:
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_REQ_SIZE, 4
	lea 4(%a6),%a2
| For the 89/92+/V200/89T DBUS ROM dumper, the compiler made an interesting optimization here: it knows which bits are
| used in ROM_size, which was computed from tios_table & 0xE00000, (ROM_base + 0xFFFFFUL) & 0x00F00000UL or hard-coded.
| In all cases, only bits 16-23 in the register contain something.
| After reversing the endianness, the nonzero bits have moved to bits 8-15.
| So the compiler just generated a lsr.l #8, which is correct, even though it doesn't seem to be at first glance !
.ifndef DUSB_DUMPER
	move.l   %d6,%d3                | scratch register preserved across function calls
	lsr.l    #8,%d3
	move.l   %d3,(%a2)
.else
| Hard-coded ROM size for the 89T, with endianness reversed. Not smaller than the 3-instruction sequence right above.
	move.l   #0x00004000,(%a2)
.endif
	COMPUTE_CHECKSUM %d5, CMD_REQ_SIZE, 4
	moveq    #4,%d4                 | len
	bsr.s    _SendPacket
__mark_end_Send_SIZE:
| sprintf(str, "Size: %lu KB", ROM_size >> 10); 12 bytes on the stack
.ifndef DUSB_DUMPER
	lsr.w    #2,%d3                 | We already shifted it by 8 previously, and the value was preserved by the call to _SendPacket
	move.l   %d3,-(%sp)
.else
	pea      4096.w                 | 0x400000 >> 10 = 0x1000. Again, not smaller than the 2-instruction sequence right above.
.endif
	pea      __str_Size(%pc)
	pea      -STR_SIZE(%a6)
	move.l   sprintf(%a5),%a0
	jsr      (%a0)
| DrawStr(0, 60, str, A_REPLACE); 10 - 2 bytes on the stack
	move.w   #4,(%sp)
	pea      -STR_SIZE(%a6)
	pea      60.w
	movea.l  DrawStr(%a5),%a0
	jsr      (%a0)
	lea      (12 + (10 - 2))(%sp),%sp
	bra.w    __main_loop_start
__do_Handle_REQ_BLOCK:
__mark_begin_cmd_req_block:
| Read requested address, reverse endianness and compare against ROM_size. Store to call-preserved register.
	move.l   4(%a6),%d3
	rol.w    #8,%d3
	swap     %d3
	rol.w    #8,%d3
	cmp.l    %d6,%d3
	bls.s    __received_address_is_alright
__mark_begin_Send_ERR:
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_ERROR, 0
	| No need to set a2 in this path, since we aren't going to use it because len == 0.
	|suba.l   %a2,%a2
	COMPUTE_CHECKSUM %d5, CMD_ERROR, 0
	moveq    #0,%d4                 | len
	bsr      _SendPacket
	bra.s    __end_cmd_req_block
__mark_end_Send_ERR:
__received_address_is_alright:
| sprintf(str, "Done: %lu/%luKB     ", addr >> 10, ROM_size >> 10); 16 bytes on the stack
| sprintf(str, "Size: %lu KB", ROM_size >> 10); 
.ifndef DUSB_DUMPER
	pea      4096.w                 | 0x400000 >> 10 = 0x1000
	moveq    #10,%d1
.else
	move.l   %d6,%d0                | ROM_size -> scratch register
	moveq    #10,%d1
	lsr.l    %d1,%d0
	move.l   %d0,-(%sp)             | ROM_size >> 10
.endif
	move.l   %d3,%d0                | addr -> scratch register
	lsr.l    %d1,%d0
	move.l   %d0,-(%sp)             | addr >> 10
	pea      __str_Done(%pc)
	pea      -STR_SIZE(%a6)
	move.l   sprintf(%a5),%a0
	jsr      (%a0)
| DrawStr(0, 60, str, A_REPLACE); 10 - 2 bytes on the stack
	move.w   #4,(%sp)
	pea      -STR_SIZE(%a6)
	pea      60.w
	movea.l  DrawStr(%a5),%a0
	jsr      (%a0)
	lea      (16 + (10 - 2))(%sp),%sp

	movea.l  %d3,%a2                | addr to scratch register
	adda.l   %d7,%a2                | ptr = ROM_base + addr
	movea.l  %a2,%a0
	move.b   (%a0)+,%d0             | load ptr[0]
	move.w   #BLK_SIZE - 1,%d1      | loop iteration count
__loop_check_for_repeated_data:
	cmp.b    (%a0)+,%d0
	dbne     %d1,__loop_check_for_repeated_data
	bne.s    __no_repeated_data
__mark_begin_Send_RDATA:
| If we reach this instruction, we have found BLK_SIZE consecutive occurrences of the same byte. We can send a short message.
	lea      6(%a6),%a2
	lsl.w    #8,%d0                 | value as little-endian, which also clears the unwanted part of the word.
	move.w   %d0,(%a2)
	move.w   #(BLK_SIZE >> 8),-(%a2) | BLK_SIZE as little-endian.
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_DATA2, 4
	COMPUTE_CHECKSUM %d5, CMD_DATA2, 4
	moveq    #4,%d4                 | len
	bsr      _SendPacket
__mark_end_Send_RDATA:
	bra.s    __end_cmd_req_block
__no_repeated_data:
__mark_begin_Send_DATA:
	| addr is already in a2
	COMPUTE_AND_WRITE_LE_CMD_AND_LE_LEN %a6, CMD_DATA1, BLK_SIZE
	COMPUTE_CHECKSUM %d5, CMD_DATA1, BLK_SIZE
	move.w   #BLK_SIZE,%d4          | len
	bsr      _SendPacket	;#
__mark_end_Send_DATA:
__end_cmd_req_block:
__mark_end_cmd_req_block:
	bra.w    __main_loop_start	;#
__main_loop_end:
__mark_after_loop:

.ifdef DUSB_DUMPER
| USBLinkClose(), but reuse RecvData pointer to get there.
	jsr      -(266 - 246)(%a4)           | RecvData is at CheckStatus - 246 and USBLinkClose is at CheckStatus - 266
.endif

| Restore sp to either saved registers (Fargo) or LCD buffer (otherwise).
	lea      STACK_OFFSET_ABOVE_A6(%a6),%sp

.ifndef FARGO
| Restore the screen
| Execute the loop LCD_SIZE/4 times.
	move.w   #(3840/4)-1,%d0
| LCD_MEM + LCD_SIZE.
	lea.l    (LCD_MEM + LCD_SIZE).w,%a0
| Pop data from the stack.
0:
	move.l   (%sp)+,-(%a0)
	dbf      %d0,0b
.endif

__restore_regs_and_return:
	movem.l  (%sp)+,%d3-%d7/%a2-%a6
	rts

__str_ROMDumper_VERSION:
	.ascii "RomDumper v2.00"
.ifdef DUSB_DUMPER
	.ascii " USB"
.endif
	.byte 0x00
__str_Type:
.ifdef FARGO
	.ascii "Type: TI-92 %d\0"
.else
	.ascii "Type: HW%d\0"
.endif
__str_ROMbase:
	.ascii "ROM base: 0x%lx\0"
__str_Credits:
	.ascii "by The TILP Team\0"
__str_Size:
	.ascii "Size: %lu KB\0"
__str_Done:
	.ascii "Done: %lu/%luKB     \0"

| Footer for environments without special TI-68k format handling.
.ifdef FARGO

.ifndef TIGCC_ENV
	.global __footer, __end_of_program_data
__footer:
	.byte 0x00
__fargo_relocs:
	.byte 0x00
__fargo_bss_refs:
	.word 0x00
__fargo_libs:
	.word 0x00

	.byte 0xE9             | ENDSTACK_TAG
	.byte 0x12, 0xE4       | 'EndPrgm'
	.byte 0x00, 0xE8       | Newline
	.byte 0x19, 0xE4       | 'Prgm'
	.byte 0xE5             | No arguments, END_TAG only
	.byte 0x00, 0x00, 0x40 | Flags

	.byte 0xDC | FUNC_TAG
__end_of_program_data:

.endif

.else

.ifndef TIGCC_ENV
	.global __footer, __end_of_program_data
	.even
__footer:
	.word 0    | Empty AMS relocation table.
	.byte 0xF3 | ASM_TAG
__end_of_program_data:

.endif

.endif | FARGO
