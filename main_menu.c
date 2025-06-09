#include "menu.h"

typedef enum EFI_MAIN_MENU_OPTIONS
{
    EfiMainMenuScreenInfoMenu,
    EfiMainMenuKernelStart,
    EfiMainMenuSubMenu1,
    EfiMainMenuSubMenu2,
    EfiMainMenuSubMenu3,
    EfiMainMenuExit,
    EfiMainMenuN
} EFI_MAIN_MENU_OPTIONS;

EFI_MENU_STATE MainMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    // Select submenu
    This->PrevOption = This->CurrentOption;
    if (Key->UnicodeChar == UnicodeCharNewline)
    {
        switch (This->CurrentOption)
        {
        case EfiMainMenuScreenInfoMenu:
            return EfiScreenInfoMenuState;
        case EfiMainMenuKernelStart:
        {
            break;
        }
        case EfiMainMenuExit:
            return EfiExitState;
        }
    }

    if (Key->ScanCode == ScanCodeEscape)
        return EfiExitState;

    // Move selection
    if (Key->ScanCode == ScanCodeArrowDown)
        This->CurrentOption++;
    else if (Key->ScanCode == ScanCodeArrowUp)
        This->CurrentOption--;

    // Check current selection bounds
    if (This->CurrentOption < 0)
        This->CurrentOption = 0;
    else if (This->CurrentOption > EfiMainMenuN - 1)
        This->CurrentOption = EfiMainMenuN - 1;

    return EfiMainMenuState;
}

VOID MainMenuUpdate(EFI_MENU_PAGE *This)
{
    CHAR16 *Options[EfiMainMenuN] = {
        [EfiMainMenuScreenInfoMenu] = u"Screen Info",
        [EfiMainMenuKernelStart] = u"Launch Kernel",
        [EfiMainMenuSubMenu1] = u"SubMenu 1",
        [EfiMainMenuSubMenu2] = u"SubMenu 2",
        [EfiMainMenuSubMenu3] = u"SubMenu 3",
        [EfiMainMenuExit] = u"Exit",
    };
    INT32 TopSelectableRow = cOut->Mode->CursorRow;

    if (This->RedrawNeeded)
    {
        cOut->ClearScreen(cOut);
        This->RedrawNeeded = FALSE;
        Printf(u"===== Main Menu =====\r\n\n");

        TopSelectableRow = cOut->Mode->CursorRow;

        for (INT32 i = 0; i < EfiMainMenuN; i++)
        {
            cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
            if (i == This->CurrentOption)
                cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));

            Printf(u"%s\r\n", Options[i]);
        }
        cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

        cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
        return;
    }

    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + This->PrevOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    Printf(u"%s\r", Options[This->PrevOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + This->CurrentOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
    Printf(u"%s\r", Options[This->CurrentOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
}

// Initialize MainMenu Page
EFI_MENU_PAGE *MainMenu(VOID)
{
    EFI_MENU_PAGE *MainMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID **)&MainMenuPtr);
    *MainMenuPtr = DefaultPage;
    MainMenuPtr->ProcessInput = MainMenuProcessInput;
    MainMenuPtr->Update = MainMenuUpdate;
    return MainMenuPtr;
}
