#include "efi.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle, (void)SystemTable;
    EFI_STATUS Status = EFI_COMPROMISED_DATA;
    if (EFI_ERROR(Status))
        return Status;

    return EFI_SUCCESS;
}
