#include "efilib.h"
#include "menu.h"

EFI_STATUS UefiEntry(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; // Unused for now

    InitGlobalVars(SystemTable);

    // Clear screen, set cursor position to (0,0), etc...
    Status = cOut->Reset(cOut, FALSE);

    // Set colors: blue letters, gray background
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    cOut->ClearScreen(cOut);

    EFI_MENU_PAGE *Menus[2] = {
        (EFI_MENU_PAGE*)MainMenu(),
        (EFI_MENU_PAGE*)OtherMenu()
    };

    // Start main event loop
    EFI_MENU_PAGE *CurrentMenu = Menus[EfiMainMenuState];
    EFI_INPUT_KEY Key;
    UINTN Count = 0;
    while (TRUE) {
        cIn->ReadKeyStroke(cIn, &Key);
        EFI_MENU_STATE NextMenuState = CurrentMenu->ProcessInput(CurrentMenu, &Key);

        if (NextMenuState == EfiExitState)
            break;

        CurrentMenu = Menus[NextMenuState];
        CurrentMenu->Update(CurrentMenu);

        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        cOut->ClearScreen(cOut);
    }

    RS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return EFI_SUCCESS;
}
