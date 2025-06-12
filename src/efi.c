#include "efilib.h"
#include "menu.h"

EFI_STATUS UefiEntry(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    (void)ImageHandle; // Unused for now

    InitGlobalVars(SystemTable);

    // Clear screen, set cursor position to (0,0), etc...
    cOut->Reset(cOut, FALSE);

    // Set colors: blue letters, gray background
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    cOut->ClearScreen(cOut);

    EFI_MENU_PAGE *MenuStates[] = {
        [EfiMainMenuState]       = MainMenu(),
        [EfiScreenInfoMenuState] = ScreenInfoMenu(),
        [EfiGOPInfoMenuState]    = GOPInfoMenu()};

    // Start main event loop
    EFI_MENU_PAGE *PrevMenuState = NULL;
    EFI_MENU_PAGE *CurrentMenuState = MenuStates[EfiMainMenuState];
    EFI_INPUT_KEY Key;
    while (TRUE)
    {
        cIn->ReadKeyStroke(cIn, &Key);
        EFI_MENU_STATE NextMenuState = CurrentMenuState->ProcessInput(CurrentMenuState, &Key);

        if (NextMenuState == EfiExitState)
            break;

        CurrentMenuState = MenuStates[NextMenuState];
        if (CurrentMenuState != PrevMenuState)
            CurrentMenuState->RedrawNeeded = TRUE;

        CurrentMenuState->Update(CurrentMenuState);

        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        PrevMenuState = CurrentMenuState;
    }

    RS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return EFI_SUCCESS;
}
