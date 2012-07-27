EXTRA_DIST = *.c *.h *.tpr
CLEANFILES = *.89z *.9xz *.v2z *.92p

TIGCCFLAGS = -Os -ffunction-sections -fdata-sections -Wall -W -Wwrite-strings  -WA,-g,-t --optimize-nops --optimize-returns --optimize-branches --optimize-moves --optimize-tests --optimize-calcs --remove-unused --cut-ranges --reorder-sections --merge-constants --omit-bss-init

if BUILD_68K_ROM_DUMPERS
all: romdump.89z romdump.92p
endif

romdump.89z: romdump.c romdump.h
	tigcc romdump.c -o "romdump" -n RomDump $(TIGCCFLAGS) -DUSE_TI89 -DUSE_TI92PLUS -DUSE_V200 -DMIN_AMS=100 -DUNOFFICIAL_OS_SUPPORT -DOPTIMIZE_ROM_CALLS -DKERNEL_FORMAT_BSS -DKERNEL_FORMAT_DATA_VAR -DSAVE_SCREEN --verbose

romdump.92p: romdump.c romdump.h fargodef.h
	tigcc romdump.c -o "romdump" -n RomDump $(TIGCCFLAGS) --fargo -DKERNEL_FORMAT_BSS -DKERNEL_FORMAT_DATA_VAR --verbose
