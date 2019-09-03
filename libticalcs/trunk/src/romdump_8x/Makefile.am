EXTRA_DIST = *.asm *.inc
CLEANFILES = *.bin *.lst *.73p *.82y *.83p *.8xp *.85s *.86p

PASMOFLAGS =

rom_dumpers = dump73.73p dump82.82y dump83.83p dump83p.8xp dump84pc.8xp dump85.85s dump86.86p

if BUILD_Z80_ROM_DUMPERS
all: $(rom_dumpers)
endif

dump73.bin: romdump.asm romdump.inc ti73defs.inc defs.inc main.asm \
 unlock73.asm packet.asm link.asm linksw.asm data.asm
	$(PASMO) $(PASMOFLAGS) -E TI73 -E CALC_FLASH -I $(srcdir) $(srcdir)/romdump.asm dump73_.bin
	sed -e "s/\xaa\xaa//g" < dump73_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump73.bin

dump82.bin: romdump.asm romdump.inc ti82defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display82.asm data.asm \
 font.asm
	$(PASMO) $(PASMOFLAGS) -E TI82 -E NEED_DISPLAY_ROUTINES -I $(srcdir) $(srcdir)/romdump.asm dump82_.bin
	sed -e "s/\xaa\xaa//g" < dump82_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump82.bin

dump83.bin: romdump.asm romdump.inc ti83defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm
	$(PASMO) $(PASMOFLAGS) -E TI83 -I $(srcdir) $(srcdir)/romdump.asm dump83_.bin
	sed -e "s/\xaa\xaa//g" < dump83_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump83.bin

dump83p.bin: romdump.asm romdump.inc ti83pdefs.inc defs.inc main.asm \
 unlock83p.asm packet.asm link.asm link83p.asm data.asm
	$(PASMO) $(PASMOFLAGS) -E TI83P -E CALC_FLASH -E CALC_LINK_ASSIST -I $(srcdir) $(srcdir)/romdump.asm dump83p_.bin
	sed -e "s/\xaa\xaa//g" < dump83p_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump83p.bin

dump84pc.bin: romdump.asm romdump.inc ti84pcdefs.inc defs.inc main.asm \
 packet.asm link.asm link83p.asm data.asm
	$(PASMO) $(PASMOFLAGS) -E TI84PC -E CALC_LINK_ASSIST -I $(srcdir) $(srcdir)/romdump.asm dump84pc_.bin
	sed -e "s/\xaa\xaa//g" < dump84pc_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump84pc.bin

dump85.bin: romdump.asm romdump.inc ti85defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display85.asm data.asm \
 font.asm
	$(PASMO) $(PASMOFLAGS) -E TI85 -E NEED_DISPLAY_ROUTINES -I $(srcdir) $(srcdir)/romdump.asm dump85_.bin
	sed -e "s/\xaa\xaa//g" < dump85_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump85.bin

dump86.bin: romdump.asm romdump.inc ti86defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm
	$(PASMO) $(PASMOFLAGS) -E TI86 -I $(srcdir) $(srcdir)/romdump.asm dump86_.bin
	sed -e "s/\xaa\xaa//g" < dump86_.bin | sed -e "s/\xaa\xe9/\xe9/g" > dump86.bin

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
