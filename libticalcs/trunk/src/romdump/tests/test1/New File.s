	.file	"New File.c"
#NO_APP
	.data
tigcc_compiled.:
#APP
/* Main Startup Code */
	.even
	.xdef __save__sp__
	/* Screen Saving Support */
	pea.l (%a2)
	lea (%sp,-3840),%sp
	pea 3840
	pea 0x4C00
	pea (%sp,8)
	move.l 0xC8,%a0
	move.l (%a0,0x26A*4),%a2
	jsr (%a2) /* memcpy */
	/* exit and atexit fix */
	movem.l %d3-%d7/%a2-%a6,-(%a7)
	jbsr _main
	movem.l (%a7)+,%d3-%d7/%a2-%a6
	/* Screen Saving Support (Restoring) */
	pea 3840
	pea (%sp,16)
	pea 0x4C00
	jsr (%a2)
	lea (%sp,3864),%sp
	movea.l (%a7)+,%a2
	rts
__save__sp__:
	.long 0
	.even
#NO_APP
#APP
	xdef _ti89
	/* Include Patch: nostub_patch */
	/* Include Patch: nostub_optimize_rom_calls */
	/* Include Patch: nostub_save_screen */
	/* Include Patch: no_retval */
	/* Include Patch: nostub_exit_support */
	/* Include Patch: save_the_sp */
	/* Include Patch: complex_main */
	.set MT_TEXT,0x8000
	.set MT_XREF,0x9000
	.set MT_ICON,0xA000
	.set MT_CASCADE,0x4000
#NO_APP
	.data
	.even
	.globl	_main
_main:
#APP
	move.l %a7,__save__sp__
	/* Support for OPTIMIZE_ROM_CALLS */
	move.l 0xC8,%a5
#NO_APP
	link.w %a6,#0
	unlk %a6
	rts
.comm _nostub,2
