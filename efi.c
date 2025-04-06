#include "efi.h"

EFI_STATUS Status;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle;
    EFI_SYSTEM_TABLE  *ST = SystemTable;
    EFI_BOOT_SERVICES *BS = ST->BootServices;

    // Clear screen, set cursor position to (0,0), etc...
    Status = ST->ConOut->Reset(ST->ConOut, FALSE);

    INT32 TextMode = SystemTable->ConOut->Mode->Mode;
    CHAR16 mode = (CHAR16)TextMode + 48;
    ST->ConOut->OutputString(ST->ConOut, u"DEVICE INFO:\r\n\tTEXT_MODE: ");
    ST->ConOut->OutputString(ST->ConOut, &mode);
    ST->ConOut->OutputString(ST->ConOut, u"\r\n");

    ST->ConOut->OutputString(ST->ConOut, u"Press a key...\r\n");

    // Wait until key has been pressed
    BS->WaitForEvent(1, &ST->ConIn->WaitForKey, NULL);

    // Read in the key and print it
    EFI_INPUT_KEY Key;
    ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
    ST->ConOut->OutputString(ST->ConOut, u"You pressed: '");
    ST->ConOut->OutputString(ST->ConOut, &Key.UnicodeChar);
    ST->ConOut->OutputString(ST->ConOut, u"'\r\n");

    // Wait until user presses key to exit
    ST->ConOut->OutputString(ST->ConOut, u"Press any key to exit...\r\n");
    BS->WaitForEvent(1, &ST->ConIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
