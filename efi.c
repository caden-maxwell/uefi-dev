#include "efi.h"

EFI_STATUS Status;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle;

    // Clear screen, set cursor position to (0,0), etc...
    Status = SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);

    CHAR16 *string = u"Hello, world!\r\n";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, string);

    string = u"Press a key and see what happens: ";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, string);

    // Wait until key has been pressed
    SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, NULL);

    // Read in the key and print it
    EFI_INPUT_KEY Key;
    Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, &Key.UnicodeChar);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    // Wait until user presses key to exit
    string = u"Press any key to exit...\r\n";
    SystemTable->ConOut->OutputString(SystemTable->ConOut, string);
    SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
