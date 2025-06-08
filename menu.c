#include "efilib.h"
#include "menu.h"

typedef enum EFI_MAIN_MENU_OPTIONS {
    EfiMainMenuScreenInfoMenu,
    EfiMainMenuExit,
    EfiMainMenuN
} EFI_MAIN_MENU_OPTIONS;

const EFI_MENU_PAGE DefaultPage = {
    .RedrawNeeded = TRUE,
    .CurrentOption = 0,
    .PrevOption = 0,
    .ProcessInput = NULL,
    .Update = NULL,
    .AwaitingInput = FALSE,
    .InputBuffer = u"\0",
    .InputLength = 0,
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
            case EfiMainMenuScreenInfoMenu: return EfiScreenInfoMenuState;
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
        [EfiMainMenuScreenInfoMenu] = u"Screen Info",
        [EfiMainMenuExit]           = u"Exit",
    };
    INT32 TopRow = cOut->Mode->CursorRow;
    CHAR16 *Arrow = u" <-";
    if (This->RedrawNeeded)
    {
        cOut->ClearScreen(cOut);
        This->RedrawNeeded = FALSE;
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

// ==============================
// ====== Screen Info Menu ======
// ==============================

typedef enum EFI_SCREEN_INFO_MENU_OPTIONS {
    EfiScreenInfoMenuSetTextMode,
    EfiScreenInfoMenuBack,
    EfiScreenInfoMenuN
} EFI_SCREEN_INFO_MENU_OPTIONS;

EFI_MENU_STATE ScreenInfoMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    // Printf(u"KEY: %c, SCANCODE: 0x%x\r\n", Key->UnicodeChar, Key->ScanCode);
    // Select submenu/perform action
    This->PrevOption = This->CurrentOption;
    if (This->AwaitingInput) {
        if (Key->UnicodeChar == UnicodeCharNewline)
        {
            This->AwaitingInput = FALSE;
            This->RedrawNeeded = TRUE;
            UINTN num;
            // This *should* be protected against invalid inputs like 'u' or ' '
            StrToUInt(This->InputBuffer, &num);
            cOut->SetMode(cOut, num);
        }
        else if (Key->UnicodeChar >= '0' && Key->UnicodeChar <= '9' && Key->UnicodeChar - '0' < cOut->Mode->MaxMode)
            This->InputBuffer[0] = Key->UnicodeChar;

        This->InputBuffer[1] = '\0';
    }
    else if (Key->UnicodeChar == UnicodeCharNewline) {
        switch (This->CurrentOption) {
            case EfiScreenInfoMenuSetTextMode:  This->AwaitingInput = TRUE; break;
            case EfiScreenInfoMenuBack: return EfiMainMenuState;
        }
    }
    // Move selection
    else if (Key->ScanCode == ScanCodeArrowDown)
        This->CurrentOption++;
    else if (Key->ScanCode == ScanCodeArrowUp)
        This->CurrentOption--;

    // Check current selection bounds
    if (This->CurrentOption < 0)
        This->CurrentOption = 0;
    else if (This->CurrentOption > EfiScreenInfoMenuN - 1)
        This->CurrentOption = EfiScreenInfoMenuN - 1;

    return EfiScreenInfoMenuState;
}

VOID ScreenInfoMenuUpdate(EFI_MENU_PAGE *This)
{
    CHAR16 *Options[EfiScreenInfoMenuN] = {
        [EfiScreenInfoMenuSetTextMode] = u"Set Text Mode",
        [EfiScreenInfoMenuBack]        = u"Back to Main Menu",
    };
    INT32 TopRow = cOut->Mode->CursorRow;
    CHAR16 *Arrow = u" <-";
    if (This->RedrawNeeded || This->AwaitingInput)
    {
        This->RedrawNeeded = FALSE;
        cOut->ClearScreen(cOut);
        This->DebugInfo.NumDraws++;

        UINTN cols = 0;
        UINTN rows = 0;
        cOut->QueryMode(cOut, cOut->Mode->Mode, &cols, &rows);
        Printf(
            u"===== Screen Info =====\r\n\n"
            u"Current Mode: %d\r\n\n"
            u"MaxMode: %d\r\n"
            u"CursorRow: %d\r\n"
            u"CursorColumn: %d\r\n"
            u"CursorVisible: %d\r\n"
            u"Columns: %d\r\n"
            u"Rows: %d\r\n\n",
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

        TopRow = cOut->Mode->CursorRow;

        CHAR16 Suffix[16];
        for (INT32 i=0; i < EfiScreenInfoMenuN; i++)
        {
            Suffix[0] = '\0';
            if (i == This->CurrentOption) {
                strlcpy(Suffix, Arrow, 4);
                if (This->AwaitingInput)
                    sPrintf(Suffix, u" (0-%d): %s", cOut->Mode->MaxMode-1, This->InputBuffer);
            }

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

// Initialize ScreenInfoMenu Page
EFI_MENU_PAGE *ScreenInfoMenu(VOID)
{
    EFI_MENU_PAGE *ScreenInfoMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID**)&ScreenInfoMenuPtr);
    *ScreenInfoMenuPtr = DefaultPage;
    ScreenInfoMenuPtr->ProcessInput = ScreenInfoMenuProcessInput;
    ScreenInfoMenuPtr->Update = ScreenInfoMenuUpdate;

    const INT32 tmplen = 2;
    CHAR16 tmp[tmplen];
    IntToStr(tmp, cOut->Mode->Mode);
    strlcpy(ScreenInfoMenuPtr->InputBuffer, tmp, tmplen);
    return ScreenInfoMenuPtr;
}
