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

    Printf(u"--------------------------------\r\n");
    Printf(u"---- WELCOME TO MY UEFI APP ----\r\n");
    Printf(u"--------------------------------\r\n\n");
    Printf(u"GREETING: '%s'\r\n", u"HELLO, WORLD!");
    Printf(u"Press any key for input...\r\n");

    // Wait until key has been pressed
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    // Read in the keystroke and print it
    EFI_INPUT_KEY Key;
    cIn->ReadKeyStroke(cIn, &Key);
    Printf(u"You pressed: '%c' --- %s!\r\n", Key.UnicodeChar, u"Congratulations");

    // Wait until user presses key to exit
    Printf(u"Press any key to exit...\r\n");
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
