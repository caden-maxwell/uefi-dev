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

    EFI_EVENT Timer;
    BS->CreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, NULL, &Timer);

    UINT32 units_per_sec = 10000000;
    INT32 exit_seconds = 60;
    BS->SetTimer(Timer, TimerPeriodic, 60 * units_per_sec);

    Printf(u"Exiting in %d seconds, or press any key to exit...\r\n", exit_seconds);

    EFI_EVENT Events[] = { Timer, cIn->WaitForKey };
    
    UINTN Index;
    BS->WaitForEvent(2, Events, &Index);

    EFI_INPUT_KEY Key;
    cIn->ReadKeyStroke(cIn, &Key);

    Printf(u"Event %d was fired: ", Index);
    if (Index == 0)
        Printf(u"Timer ran out.");
    else
        Printf(u"Key '%c' was pressed.", Key.UnicodeChar);


    BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
    cIn->ReadKeyStroke(cIn, &Key);

    RS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return EFI_SUCCESS;
}
