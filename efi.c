#include "efi.h"

EFI_STATUS Status;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle;

    // Clear screen, set cursor position to (0,0), etc...
    Status = SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);

    CHAR16 *string = u"Hello, world!\r\n";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, string);

    string = u"Press any key to exit...\r\n";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, string);

    // Keep reading until key has been pressed
    EFI_INPUT_KEY Key;
    Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    while (EFI_ERROR(Status))
        Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);

    return EFI_SUCCESS;
}
