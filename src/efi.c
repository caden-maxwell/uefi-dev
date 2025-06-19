#include "efilib.h"
#include "menu.h"

EFI_STATUS UefiEntry(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitGlobalVars(ImageHandle, SystemTable);

    // Clear screen, set cursor position to (0,0), etc...
    cOut->Reset(cOut, FALSE);

    // Set colors: blue letters, gray background
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    cOut->ClearScreen(cOut);

    EFI_MENU_PAGE *Menus[] = {
        [EfiMainMenuState]       = MainMenu(),
        [EfiScreenInfoMenuState] = ScreenInfoMenu(),
        [EfiGOPInfoMenuState]    = GOPInfoMenu(),
        [EfiKernelStartState]    = KernelStartMenu()
    };

    // Start main event loop
    EFI_MENU_PAGE *PrevMenus = NULL;
    EFI_MENU_PAGE *CurrentMenus = Menus[EfiMainMenuState];
    EFI_INPUT_KEY Key;
    while (TRUE)
    {
        cIn->ReadKeyStroke(cIn, &Key);
        EFI_MENU_STATE NextMenu = CurrentMenus->ProcessInput(CurrentMenus, &Key);

        if (NextMenu == EfiExitState)
            break;

        CurrentMenus = Menus[NextMenu];
        if (CurrentMenus != PrevMenus)
            CurrentMenus->RedrawNeeded = TRUE;

        CurrentMenus->Update(CurrentMenus);

        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        PrevMenus = CurrentMenus;
    }

    for (UINT32 i=0; i < ARRAY_LEN(Menus); i++)
        BS->FreePool(Menus[i]);

    RS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    // Should not get here
    return EFI_SUCCESS;
}
