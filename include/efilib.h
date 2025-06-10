#include "efi.h"

#ifndef _EFILIB_H
#define _EFILIB_H

// Macros
#define FALSE (BOOLEAN)(0 == 1)
#define TRUE (BOOLEAN)(0 == 0)
#define NULL 0

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define sPrintfSafe(dst, fstr, ...) snPrintf(dst, ARRAY_LEN(dst), fstr, __VA_ARGS__)
#define StrCpySafe(dst, src) StrlCpy(dst, src, ARRAY_LEN(dst))

// Globals
extern EFI_SYSTEM_TABLE *ST;
extern EFI_BOOT_SERVICES *BS;
extern EFI_RUNTIME_SERVICES *RS;
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
extern EFI_STATUS Status;

// Functions
VOID InitGlobalVars(IN EFI_SYSTEM_TABLE *SystemTable);
VOID Printf(IN CHAR16 *fstr, ...);
VOID snPrintf(OUT CHAR16 *dest, IN UINTN destsize, IN CHAR16 *fstr, ...);
VOID PutChar(IN CHAR16 ch);
UINTN StrlCpy(OUT CHAR16 *dest, IN const CHAR16 *source, IN UINTN size);
VOID IntToStr(OUT CHAR16 *dest, IN INT32 num);
BOOLEAN StrToUInt(IN CHAR16 *str, OUT UINTN *out);
UINTN pow(IN UINTN base, IN UINTN exponent);

#endif //_EFILIB_H
