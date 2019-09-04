EXTRA_DIST = *.asm *.inc
CLEANFILES = *.bin *.lst *.73p *.82y *.83p *.8xp *.85s *.86p

SPASMFLAGS = -N -T

rom_dumpers = dump73.73p dump82.82y dump83.83p dump83p.8xp dump84pc.8xp dump85.85s dump86.86p

if BUILD_Z80_ROM_DUMPERS
all: $(rom_dumpers)
endif

dump73.bin: romdump.asm romdump.inc ti73defs.inc defs.inc main.asm \
 unlock73.asm packet.asm link.asm linksw.asm data.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI73 -I $(srcdir) $(srcdir)/romdump.asm dump73.bin

dump82.bin: romdump.asm romdump.inc ti82defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display82.asm data.asm \
 font.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI82 -I $(srcdir) $(srcdir)/romdump.asm dump82.bin

dump83.bin: romdump.asm romdump.inc ti83defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI83 -I $(srcdir) $(srcdir)/romdump.asm dump83.bin

dump83p.bin: romdump.asm romdump.inc ti83pdefs.inc defs.inc main.asm \
 unlock83p.asm packet.asm link.asm link83p.asm data.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI83P -I $(srcdir) $(srcdir)/romdump.asm dump83p.bin

dump84pc.bin: romdump.asm romdump.inc ti84pcdefs.inc defs.inc main.asm \
 packet.asm link.asm link83p.asm data.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI84PC -I $(srcdir) $(srcdir)/romdump.asm dump84pc.bin

dump85.bin: romdump.asm romdump.inc ti85defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm display.asm display85.asm data.asm \
 font.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI85 -I $(srcdir) $(srcdir)/romdump.asm dump85.bin

dump86.bin: romdump.asm romdump.inc ti86defs.inc defs.inc main.asm \
 packet.asm link.asm linksw.asm data.asm
	$(SPASM) $(SPASMFLAGS) -A -DTI86 -I $(srcdir) $(srcdir)/romdump.asm dump86.bin

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
