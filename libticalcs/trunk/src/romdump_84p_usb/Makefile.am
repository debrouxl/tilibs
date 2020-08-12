EXTRA_DIST = romdump.z80
CLEANFILES = *.bin *.lst *.8xp

PASMOFLAGS =

rom_dumpers = dump84p.8xp dump84pcu.8xp dump82a.8xp
if BUILD_Z80_ROM_DUMPERS
all: $(rom_dumpers)
endif

dump84p.bin: romdump.z80
	$(PASMO) $(PASMOFLAGS) -E TI84P $(srcdir)/romdump.z80 dump84p.bin

dump84pcu.bin: romdump.z80
	$(PASMO) $(PASMOFLAGS) -E TI84PC $(srcdir)/romdump.z80 dump84pcu.bin

dump82a.bin: romdump.z80
	$(PASMO) $(PASMOFLAGS) -E TI82A $(srcdir)/romdump.z80 dump82a.bin

dump84p.8xp: dump84p.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump84p.bin -p -o dump84p.8xp

dump84pcu.8xp: dump84pcu.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump84pcu.bin -p -o dump84pcu.8xp

dump82a.8xp: dump82a.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump82a.bin -p -o dump82a.8xp
