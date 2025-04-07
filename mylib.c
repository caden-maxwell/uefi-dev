#include "efi.h"
#include "mylib.h"

EFI_SYSTEM_TABLE  *ST;
EFI_BOOT_SERVICES *BS;
EFI_RUNTIME_SERVICES *RS;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
EFI_STATUS Status;

// =======================================
// ===== Initialize Global Variables =====
// =======================================
void init_global_vars(EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = ST->BootServices;
    RS = ST->RuntimeServices;
    cIn = ST->ConIn;
    cOut = ST->ConOut;
}

BOOLEAN Printf(CHAR16 *fmt, ...) {
    return TRUE;
}
