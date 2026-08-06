EXTRA_DIST = *.asm *.inc ldscript
CLEANFILES = *.bin *.lst *.73p *.82y *.83p *.8xp *.85s *.86p *.o

rom_dumpers = dump73.73p dump82.82y dump83.83p dump83p.8xp dump84pc.8xp dump85.85s dump86.86p

if BUILD_Z80_ROM_DUMPERS
all: $(rom_dumpers)
endif

dump73.bin: romdump.asm romdump.inc ti73defs.inc defs.inc main.asm \
 unlock73.asm packet.asm link.asm linksw.asm data.asm ldscript
	$(Z80_AS) --defsym TI73=1 --defsym CALC_FLASH=1 -I $(srcdir) -o dump73.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump73.o

dump82.bin: romdump.asm romdump.inc ti82defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display82.asm data.asm \
 font.asm ldscript
	$(Z80_AS) --defsym TI82=1 --defsym NEED_DISPLAY_ROUTINES=1 -I $(srcdir) -o dump82.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump82.o

dump83.bin: romdump.asm romdump.inc ti83defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm ldscript
	$(Z80_AS) --defsym TI83=1 -I $(srcdir) -o dump83.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump83.o

dump83p.bin: romdump.asm romdump.inc ti83pdefs.inc defs.inc main.asm \
 unlock83p.asm packet.asm link.asm link83p.asm data.asm ldscript
	$(Z80_AS) --defsym TI83P=1 --defsym CALC_FLASH=1 --defsym CALC_LINK_ASSIST=1 -I $(srcdir) -o dump83p.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump83p.o

dump84pc.bin: romdump.asm romdump.inc ti84pcdefs.inc defs.inc main.asm \
 packet.asm link.asm link83p.asm data.asm ldscript
	$(Z80_AS) --defsym TI84PC=1 --defsym CALC_LINK_ASSIST=1 -I $(srcdir) -o dump84pc.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump84pc.o

dump85.bin: romdump.asm romdump.inc ti85defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display85.asm data.asm \
 font.asm ldscript
	$(Z80_AS) --defsym TI85=1 --defsym NEED_DISPLAY_ROUTINES=1 -I $(srcdir) -o dump85.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump85.o

dump86.bin: romdump.asm romdump.inc ti86defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm ldscript
	$(Z80_AS) --defsym TI86=1 -I $(srcdir) -o dump86.o $(srcdir)/romdump.asm
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump86.o

dump73.73p: dump73.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump73.bin -p -o dump73.73p

dump82.82y: dump82.bin
	$(TIFILEUTIL) wrap -n '$$RD' dump82.bin -o dump82.82y

dump83.83p: dump83.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump83.bin -p -o dump83.83p

dump83p.8xp: dump83p.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump83p.bin -p -o dump83p.8xp

dump84pc.8xp: dump84pc.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump84pc.bin -p -o dump84pc.8xp

dump85.85s: dump85.bin
	$(TIFILEUTIL) wrap -n '$$RD' dump85.bin -o dump85.85s

dump86.86p: dump86.bin
	$(TIFILEUTIL) wrap -n 'ROMDump' dump86.bin -o dump86.86p
