#include "efilib.h"

EFI_STATUS UefiEntry(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; // Unused for now

    InitGlobalVars(SystemTable);

    // Clear screen, set cursor position to (0,0), etc...
    Status = cOut->Reset(cOut, FALSE);

    // Set colors: blue letters, gray background
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    cOut->ClearScreen(cOut);

    Printf(u"================================\r\n");
    Printf(u"==== WELCOME TO MY UEFI APP ====\r\n");
    Printf(u"================================\r\n\n");

    // Test printf
    Printf(u"--- Testing format specifiers ---\r\n");
    Printf(u"- s: Hello, %s\r\n", u"world!");
    Printf(u"- c: '%c' is the first letter of the alphabet.\r\n", 'A');
    Printf(u"- d: There are %d feet in a mile.\r\n", 5280);
    Printf(u"- d: -MAX_INT: %d\r\n", -2147483647);
    Printf(u"- u x: %u is 0x%x in hexadecimal.\r\n", 0xDEADBEEF, 0xDEADBEEF);
    Printf(u"- s c d u x: %s %c %u %d 0x%x\r\n", u"Hello", 'Z', 0xFFFFFFFF, 0x1000, 4096);
    Printf(u"- r: This %r ain't right...\r\n", 0);
    Printf(u"------------------------------\r\n\n");

    // Wait until key has been pressed
    Printf(u"Press any key for input...\r\n");
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    // Read in the keystroke and print it
    EFI_INPUT_KEY Key;
    cIn->ReadKeyStroke(cIn, &Key);
    Printf(u"You pressed: '%c'\r\n", Key.UnicodeChar);

    // Wait until user presses key to exit
    Printf(u"Press any key to exit...\r\n");
    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);

    return EFI_SUCCESS;
}
