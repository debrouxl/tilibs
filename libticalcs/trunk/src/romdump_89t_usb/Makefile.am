EXTRA_DIST = *.c *.h usb.txt
CLEANFILES = *.89z

TIGCCFLAGS = -Os -ffunction-sections -fdata-sections -Wall -W -Wwrite-strings  -WA,-g,-t --optimize-nops --optimize-returns --optimize-branches --optimize-moves --optimize-tests --optimize-calcs --remove-unused --cut-ranges --reorder-sections --merge-constants --omit-bss-init

if BUILD_68K_ROM_DUMPERS
all: romdump.89z
endif

romdump.89z: romdump.c romdump.h dusb.h
	tigcc romdump.c -o "romdump" -n RomDump $(TIGCCFLAGS) -DUSE_TI89 -DMIN_AMS=100 -DUNOFFICIAL_OS_SUPPORT -DOPTIMIZE_ROM_CALLS -DKERNEL_FORMAT_BSS -DKERNEL_FORMAT_DATA_VAR -DSAVE_SCREEN --verbose
