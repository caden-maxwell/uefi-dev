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

    EFI_MENU_PAGE *Menus[] = {
        [EfiMainMenuState] = MainMenu(),
        [EfiOtherMenuState] = OtherMenu()
    };

    // Start main event loop
    EFI_MENU_PAGE *PrevMenu = NULL;
    EFI_MENU_PAGE *CurrentMenu = Menus[EfiMainMenuState];
    EFI_INPUT_KEY Key;
    while (TRUE) {
        cIn->ReadKeyStroke(cIn, &Key);
        EFI_MENU_STATE NextMenuState = CurrentMenu->ProcessInput(CurrentMenu, &Key);

        if (NextMenuState == EfiExitState)
            break;

        CurrentMenu = Menus[NextMenuState];
        if (CurrentMenu != PrevMenu)
        {
            cOut->ClearScreen(cOut);
            CurrentMenu->InitialRender = TRUE;
        }

        CurrentMenu->Update(CurrentMenu);

        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        PrevMenu = CurrentMenu;
    }

    RS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return EFI_SUCCESS;
}
