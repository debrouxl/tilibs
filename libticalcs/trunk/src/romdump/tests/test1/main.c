//roms: receiving char by polling

// C Source File
// Created 18/09/02; 11:54:46

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

// Main Function
void _main(void)
{
	unsigned char uc = 0;
	int n=0;
	
	// Place your code here.
	ClrScr();
	FontSetSys(F_6x8);
	OSLinkReset();
	do
	{

		while(OSReadLinkBlock(&uc, 1) == 0);
		putchar(uc);
		n++;
		/*if(uc == 'c')
			printf("n = %i\n", n);*/
	}
	while(uc != 'X');
}
