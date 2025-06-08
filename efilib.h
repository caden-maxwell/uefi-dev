#include "efi.h"

#ifndef _EFILIB_H
#define _EFILIB_H

#define FALSE (BOOLEAN)(0==1)
#define TRUE  (BOOLEAN)(0==0)
#define NULL  0

// ----- Globals -----
extern EFI_SYSTEM_TABLE  *ST;
extern EFI_BOOT_SERVICES *BS;
extern EFI_RUNTIME_SERVICES *RS;
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
extern EFI_STATUS Status;
// ----- Globals -----

void InitGlobalVars(EFI_SYSTEM_TABLE *SystemTable);
void Printf(IN CHAR16 *fmt, ...);
void sPrintf(OUT CHAR16 *buf, IN CHAR16 *fmt, ...);
VOID snPrintf(OUT CHAR16 *buf, IN INT32 n, IN CHAR16 *fstr, ...);
void PutChar(CHAR16 ch);
UINTN strlcpy(OUT CHAR16 *dest, const CHAR16 *source, UINTN size);

#endif //_EFILIB_H
