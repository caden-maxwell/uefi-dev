#include "efi.h"
#include "efilib.h"
#include <stdarg.h>

EFI_SYSTEM_TABLE  *ST;
EFI_BOOT_SERVICES *BS;
EFI_RUNTIME_SERVICES *RS;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
EFI_STATUS Status;

// =======================================
// ===== Initialize global variables =====
// =======================================
void init_global_vars(EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = ST->BootServices;
    RS = ST->RuntimeServices;
    cIn = ST->ConIn;
    cOut = ST->ConOut;
}

// ===================================
// ===== Print formatted strings =====
// ===================================
BOOLEAN Printf(CHAR16 *fmt, ...) {
    (void)fmt;
    return TRUE;
}
