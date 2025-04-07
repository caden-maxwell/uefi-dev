#include "efi.h"
#include "efilib.h"

EFI_STATUS UefiEntry(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; // Unused for now

    InitGlobalVars(SystemTable);

    // Clear screen, set cursor position to (0,0), etc...
    Status = cOut->Reset(cOut, FALSE);

    // Set colors
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    cOut->ClearScreen(cOut);

    Printf(u"GREETING: '%s'\r\n\n", u"HELLO, WORLD!");

    cOut->OutputString(cOut, u"--------------------------------\r\n");
    cOut->OutputString(cOut, u"---- WELCOME TO MY UEFI APP ----\r\n");
    cOut->OutputString(cOut, u"--------------------------------\r\n\n");

    cOut->OutputString(cOut, u"Press a key...\r\n");

    // Wait until key has been pressed
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    // Read in the keystroke
    EFI_INPUT_KEY Key;
    cIn->ReadKeyStroke(cIn, &Key);
    
    // Print it
    cOut->OutputString(cOut, u"You pressed: ");
    cOut->OutputString(cOut, &Key.UnicodeChar);
    cOut->OutputString(cOut, u"\r\n");

    // Wait until user presses key to exit
    cOut->OutputString(cOut, u"Press any key to exit...\r\n");
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
