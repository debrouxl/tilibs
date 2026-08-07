EXTRA_DIST = romdump.z80 ldscript
CLEANFILES = *.bin *.lst *.8xp *.o

if BUILD_Z80_ROM_DUMPERS
all: dump834pceu.8xp
endif

dump834pceu.bin: romdump.z80 ldscript
	$(Z80_AS) --defsym TI834PCE=1 -march=ez80+adl -o dump834pceu.o $(srcdir)/romdump.z80
	$(Z80_LD) -T $(srcdir)/ldscript --oformat=binary -o $@ dump834pceu.o
dump834pceu.8xp: dump834pceu.bin
	$(TIFILEUTIL) wrap -n 'ROMDUMP' dump834pceu.bin -p -o dump834pceu.8xp
