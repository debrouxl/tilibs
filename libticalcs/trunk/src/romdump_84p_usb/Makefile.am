EXTRA_DIST = romdump.z80
CLEANFILES = *.bin *.lst *.8xp

SPASMFLAGS = -N -T

if BUILD_Z80_ROM_DUMPERS
all: dump84p.8xp
endif

dump84p.bin: romdump.z80
	$(SPASM) $(SPASMFLAGS) -A $(srcdir)/romdump.z80 dump84p.bin
dump84p.8xp: dump84p.bin
	$(TIPACK) -n 'ROMDUMP' dump84p.bin -p -o dump84p.8xp
