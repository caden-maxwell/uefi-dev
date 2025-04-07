#include "efi.h"

#ifndef _LIB_H
#define _LIB_H

// ----- Globals -----
extern EFI_SYSTEM_TABLE  *ST;
extern EFI_BOOT_SERVICES *BS;
extern EFI_RUNTIME_SERVICES *RS;
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
extern EFI_STATUS Status;

void init_global_vars(EFI_SYSTEM_TABLE *SystemTable);

#endif //_LIB_H
