EXTRA_DIST = *.c *.h usb.txt
CLEANFILES = *.89z *.9xz *.v2z *.92p

TIGCCFLAGS = -Os -Wall -W -Wwrite-strings -mno-bss -mpcrel -Wa,-l -Wa,--defsym,TIGCC_ENV=1 --optimize-nops --optimize-returns --optimize-branches --optimize-moves --optimize-tests --optimize-calcs --remove-unused --cut-ranges --merge-constants -fomit-frame-pointer
ELFFLAGS = -Os -s -Wall -W -Wwrite-strings -mpcrel -Wa,-l -fomit-frame-pointer -m68000 -mshort -ffreestanding -fcall-used-d0 -fcall-used-d1 -fcall-used-d2 -fcall-used-a0 -fcall-used-a1 -fcall-saved-d3 -fcall-saved-d4 -fcall-saved-d5 -fcall-saved-d6 -fcall-saved-d7 -fno-optimize-sibling-calls

if BUILD_68K_ROM_DUMPERS
all: romdump.89z romdump.92p romdumpu.89z
endif

if BUILD_68K_ROM_DUMPERS_USING_TIGCC

romdump.89z: romdump_pure_asm.s romdump.h
	tigcc romdump_pure_asm.s -o romdump -n RomDump $(TIGCCFLAGS) --verbose -save-temps -fverbose-asm

romdump.92p: romdump_pure_asm.s romdump.h
	tigcc romdump_pure_asm.s -o romdump -n RomDump $(TIGCCFLAGS) --fargo -Wa,--defsym,FARGO=1 --verbose -save-temps -fverbose-asm

romdumpu.89z: romdump_pure_asm.s romdump.h
	tigcc romdump_pure_asm.s -o romdumpu -n RomDump $(TIGCCFLAGS) -Wa,--defsym,DUSB_DUMPER=1 --verbose -save-temps -fverbose-asm

else

romdump.o: romdump_pure_asm.s
	m68k-linux-gnu-as -l -m68000 -o romdump.o romdump_pure_asm.s

romdump.bin: romdump.o
	m68k-linux-gnu-objcopy -O binary romdump.o romdump.bin

romdump.89z: romdump.bin
	$(TIFILEUTIL) wrap -n 'romdump' romdump.bin -o romdump.89z
#m68k-linux-gnu-ld -n -T ldscript --oformat=binary -o romdump.bin romdump.o

romdumpf.o: romdump_pure_asm.s
	m68k-linux-gnu-as -l -m68000 --defsym FARGO=1 -o romdumpf.o romdump_pure_asm.s

romdumpf.bin: romdumpf.o
	m68k-linux-gnu-objcopy -O binary romdumpf.o romdumpf.bin

romdump.92p: romdumpf.bin
	$(TIFILEUTIL) wrap -n 'romdump' romdumpf.bin -o romdump.92p
#m68k-linux-gnu-ld -n -T ldscript --oformat=binary -o romdumpf.bin romdumpf.o

romdumpu.o: romdump_pure_asm.s
	m68k-linux-gnu-as -l -m68000 --defsym DUSB_DUMPER=1 -o romdumpu.o romdump_pure_asm.s

romdumpu.bin: romdumpu.o
	m68k-linux-gnu-objcopy -O binary romdumpu.o romdumpu.bin

romdumpu.89z: romdumpu.bin
	$(TIFILEUTIL) wrap -n 'romdump' romdumpu.bin -o romdumpu.89z
#m68k-linux-gnu-ld -n -T ldscript --oformat=binary -o romdumpu.bin romdumpu.o

endif
