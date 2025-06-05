#include "efilib.h"
#include "menu.h"

typedef enum EFI_MAIN_MENU_OPTIONS {
    EfiMainMenuOtherMenu,
    EfiMainMenuExit,
    EfiMainMenuN
} EFI_MAIN_MENU_OPTIONS;

const EFI_MENU_PAGE DefaultPage = {
    .InitialRender = TRUE,
    .CurrentOption = 0,
    .PrevOption = 0,
    .ProcessInput = NULL,
    .Update = NULL
};

// =======================
// ====== Main Menu ======
// =======================

EFI_MENU_STATE MainMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    // Select submenu
    This->PrevOption = This->CurrentOption;
    if (Key->UnicodeChar == UnicodeCharNewline)
    {
        switch (This->CurrentOption) {
            case EfiMainMenuOtherMenu: return EfiOtherMenuState;
            case EfiMainMenuExit: return EfiExitState;
            default: Printf(u"INVALID OPTION!!!");
        }
    }

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
        [EfiMainMenuOtherMenu] = u"Other Menu",
        [EfiMainMenuExit] = u"Exit",
    };
    INT32 TopRow = cOut->Mode->CursorRow;
    CHAR16 *Arrow = u" <-";
    if (This->InitialRender)
    {
        This->InitialRender = FALSE;
        Printf(u"===== Main Menu =====\r\n\n");

        TopRow = cOut->Mode->CursorRow;

        for (INT32 i=0; i < EfiMainMenuN; i++)
        {
            CHAR16 *Suffix = u"";
            if (i == This->CurrentOption)
                Suffix = Arrow;

            Printf(u"%s%s\r\n", Options[i], Suffix);
        }

        cOut->SetCursorPosition(cOut, 0, TopRow);
        return;
    }

    cOut->SetCursorPosition(cOut, 0, TopRow + This->PrevOption);
    Printf(u"%s   \r", Options[This->PrevOption]);
    cOut->SetCursorPosition(cOut, 0, TopRow + This->CurrentOption);
    Printf(u"%s%s\r", Options[This->CurrentOption], Arrow);
    cOut->SetCursorPosition(cOut, 0, TopRow);
}

// Initialize MainMenu Page
EFI_MENU_PAGE *MainMenu(VOID)
{
    EFI_MENU_PAGE *MainMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID**)&MainMenuPtr);
    *MainMenuPtr = DefaultPage;
    MainMenuPtr->ProcessInput = MainMenuProcessInput;
    MainMenuPtr->Update = MainMenuUpdate;
    return MainMenuPtr;
}

// =============================
// ====== Some Other Menu ======
// =============================

EFI_MENU_STATE OtherMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    (void)This;
    if (Key->UnicodeChar == UnicodeCharNewline)
        return EfiMainMenuState;

    return EfiOtherMenuState;
}

VOID OtherMenuUpdate(EFI_MENU_PAGE *This)
{
    if (This->InitialRender)
    {
        UINTN cols = 0;
        UINTN rows = 0;
        cOut->QueryMode(cOut, cOut->Mode->Mode, &cols, &rows);
        This->InitialRender = FALSE;
        Printf(
            u"===== Other Stuff =====\r\n\n"
            u"Current Mode: %d\r\n\n"
            u"MaxMode: %d\r\n"
            u"CursorRow: %d\r\n"
            u"CursorColumn: %d\r\n"
            u"CursorVisible: %d\r\n"
            u"Columns: %d\r\n"
            u"Rows: %d\r\n",
            cOut->Mode->Mode,
            cOut->Mode->MaxMode,
            cOut->Mode->CursorRow,
            cOut->Mode->CursorColumn,
            cOut->Mode->CursorVisible,
            cols, rows
        );

        for (INT32 i=0; i<cOut->Mode->MaxMode; i++)
        {
            cOut->QueryMode(cOut, i, &cols, &rows);
            Printf(u"Mode %d: %d x %d\r\n", i, cols, rows);
        }

        Printf(u"Back <-\r\n");
    }
}

// Initialize OtherMenu Page
EFI_MENU_PAGE *OtherMenu(VOID)
{
    EFI_MENU_PAGE *OtherMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID**)&OtherMenuPtr);
    *OtherMenuPtr = DefaultPage;
    OtherMenuPtr->ProcessInput = OtherMenuProcessInput;
    OtherMenuPtr->Update = OtherMenuUpdate;
    return OtherMenuPtr;
}
