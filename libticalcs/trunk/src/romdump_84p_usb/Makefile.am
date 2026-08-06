EXTRA_DIST = romdump.z80 ldscript
CLEANFILES = *.bin *.lst *.8xp *.o

rom_dumpers = dump84p.8xp dump84pcu.8xp
if BUILD_Z80_ROM_DUMPERS
all: $(rom_dumpers)
endif

dump84p.bin: romdump.z80 ldscript
	$(Z80_AS) --defsym TI84P=1 -o dump84p.o $(srcdir)/romdump.z80
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump84p.o

dump84pcu.bin: romdump.z80 ldscript
	$(Z80_AS) --defsym TI84PC=1 -o dump84pcu.o $(srcdir)/romdump.z80
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump84pcu.o

dump84p.8xp: dump84p.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump84p.bin -p -o dump84p.8xp

dump84pcu.8xp: dump84pcu.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump84pcu.bin -p -o dump84pcu.8xp
