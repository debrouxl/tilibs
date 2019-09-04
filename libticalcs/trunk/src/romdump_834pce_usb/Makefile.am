EXTRA_DIST = romdump.z80
CLEANFILES = *.bin *.lst *.8xp

SPASMFLAGS = -E -N -T

if BUILD_Z80_ROM_DUMPERS
all: dump834pceu.8xp
endif

dump834pceu.bin: romdump.z80
	$(SPASM) $(SPASMFLAGS) -A -DTI834PCE $(srcdir)/romdump.z80 dump834pceu.bin
dump834pceu.8xp: dump834pceu.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump834pceu.bin -p -o dump834pceu.8xp
