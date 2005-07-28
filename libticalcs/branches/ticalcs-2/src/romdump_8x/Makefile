Z80ASM = tasm
TICONV = ticonv
CHECK85 = check85

all: romdl romdump.8xp romdump.83p romdump.82p romdump.73p romdump.85s romdump.86p

romdl: romdl.c
	$(CC) $(CFLAGS) -W -Wall -g -o romdl romdl.c `pkg-config --cflags --libs ticables`

romdump.73p: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI73
	$(TICONV) romdump.bin $@ "ROMDUMP"

romdump.82p: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI82
	$(TICONV) romdump.bin $@ "ROMDUMP"

romdump.83p: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI83
	$(TICONV) romdump.bin $@ "ROMDUMP"

romdump.8xp: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI83P
	$(TICONV) romdump.bin $@ "ROMDUMP"

romdump.85s: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI85
	$(CHECK85) romdump.bin
	$(TICONV) romdump.bin $@ "ROMDump"

romdump.86p: romdump.asm header.asm
	rm -f romdump.bin
	$(MAKE) romdump.bin CALCMODEL=TI86
	$(TICONV) romdump.bin $@ "ROMDump"

romdump.bin: romdump.asm header.asm
	$(Z80ASM) -b $(Z80ASMFLAGS) -D$(CALCMODEL) -a -80 -c -fff -o20 -s $< $@ $(CALCMODEL).lst $(CALCMODEL).exp $(CALCMODEL).sym

clean:
	rm -f romdl dump.8xp dump.83p dump.82p dump.73p dump.85s dump.86p
	rm -f *.lst
	rm -f *.exp
	rm -f *.sym
	rm -f *.bin

.PHONY: all clean
