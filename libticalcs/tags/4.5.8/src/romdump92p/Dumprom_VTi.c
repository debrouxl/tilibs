#include <tios.h>
/*#incasm "flash.inc"*/

void DisableFlashProtect(void);

int SendByte(BYTE c)
{
    while (transmit(&c,1))
    {
        if (keyPressedFlag)
            return 0;
    }
    return 1;
}

BYTE GetByte(void)
{
    BYTE c;
    
    for (;;)
    {
        if (receive(&c,1))
            break;
        if (keyPressedFlag)
            return 0xcc;
    }
    return c;
}

int SendSegment(char *ptr)
{
    WORD csum;
    int i;
    BYTE c;

    for (;;)
    {
        for (i=0,csum=0;i<1024;i++)
        {
            BYTE ch=ptr[i];
            if (!SendByte(ch)) return 0;
            csum+=(WORD)((BYTE)(ch));
        }
        if (!SendByte(csum>>8)) return 0;
        if (!SendByte(csum&0xff)) return 0;
        c=GetByte();
        if (c==0xda)
            break;
        if (c==0xcc)
            return 0;
    }
    return 1;
}

void ZapScreen(void)
{
    long *ptr=(long*)(0x4c00);
    int i;

    for (i=0;i<((30*128)/4);i++)
        *(ptr++)=0;
}

void main(void)
{
    int i;
    char *ptr=(char*)((calculator==CALC_TI89)?0x200000:0x400000);

    /*DisableFlashProtect();*/
    ZapScreen();
    FontSetSys(2);
    for (i=0;i<0x200000;i+=1024,ptr+=1024)
    {
        char str[24];
        sprintf(str,"%ldK of 2048K",i>>10);
        if (calculator==CALC_TI89)
            DrawStrXY(80-(strlen(str)<<2),45,str,4);
        else
            DrawStrXY(120-(strlen(str)<<2),59,str,4);
        if (!SendSegment(ptr))
            break;
    }
}
