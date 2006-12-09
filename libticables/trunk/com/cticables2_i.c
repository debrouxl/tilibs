/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Dec 09 08:20:50 2006
 */
/* Compiler settings for C:\sources\roms\ticables2\com\cticables2.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ICables = {0x295A0BCE,0x428D,0x4251,{0xB4,0x32,0x9D,0xE5,0x10,0x02,0x05,0xE5}};


const IID IID_ITypes = {0x03353B2B,0xA497,0x4B42,{0xA6,0xF6,0x89,0x2F,0xB9,0x45,0xBA,0x7E}};


const IID LIBID_CTICABLES2Lib = {0x0C0B9EE6,0xF05D,0x4519,{0x90,0x86,0x1E,0x4C,0xCA,0x12,0x3A,0x50}};


const CLSID CLSID_Cables = {0xEA25011A,0xAC84,0x4498,{0x9E,0x50,0x05,0xEB,0xC5,0x7A,0xE0,0x12}};


const CLSID CLSID_Types = {0x4766347A,0x54D0,0x4F60,{0xB4,0x3A,0x8F,0x8F,0x56,0x18,0xEC,0xFE}};


#ifdef __cplusplus
}
#endif

