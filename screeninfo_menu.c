#include "menu.h"

typedef enum EFI_SCREEN_INFO_MENU_OPTIONS {
    EfiScreenInfoMenuSetTextMode,
    EfiScreenInfoMenuBack,
    EfiScreenInfoMenuN
} EFI_SCREEN_INFO_MENU_OPTIONS;

EFI_MENU_STATE ScreenInfoMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    // Printf(u"KEY: %c, SCANCODE: 0x%x\r\n", Key->UnicodeChar, Key->ScanCode); // For debugging

    // Entering mode number
    if (This->AwaitingInput)
    {
        // Get the current and max modes
        INTN PrevMode = cOut->Mode->Mode;
        INTN NewMode  = PrevMode;
        INTN MaxMode  = cOut->Mode->MaxMode - 1;

        // If [Enter] or [ESC], stop getting input
        if (Key->UnicodeChar == UnicodeCharNewline || Key->ScanCode == ScanCodeEscape)
        {
            This->RedrawNeeded = TRUE; // Need to redraw to get rid of suffixes
            This->AwaitingInput = FALSE;
        }
        // If [Down], increment the mode
        else if (Key->ScanCode == ScanCodeArrowDown)
        {
            if (++NewMode > MaxMode) NewMode = MaxMode;
        }
        // If [Up], decrement the mode
        else if (Key->ScanCode == ScanCodeArrowUp)
        {
            if (--NewMode < 0) NewMode = 0;
        }
        // If [1-MaxMode], switch to mode
        else if (Key->UnicodeChar > '0' && Key->UnicodeChar - '0' - 1 <= MaxMode)
        {
            NewMode = Key->UnicodeChar - '0' - 1;
        }

        // If the mode changed, set it and update the screen
        if (NewMode != PrevMode) {
            cOut->SetMode(cOut, NewMode);
            IntToStr(This->InputBuffer, ++NewMode);
            This->RedrawNeeded = TRUE;
        }

    // Select submenu/perform action
    } else {
        if (Key->UnicodeChar == UnicodeCharNewline) {
            switch (This->CurrentOption) {
                case EfiScreenInfoMenuSetTextMode: {
                    This->AwaitingInput = TRUE;
                    This->RedrawNeeded  = TRUE;
                    break;
                }
                case EfiScreenInfoMenuBack: return EfiMainMenuState;
            }
        }
        else if (Key->ScanCode == ScanCodeEscape) return EfiMainMenuState;

        This->PrevOption = This->CurrentOption;

        // Move selection
        if (Key->ScanCode == ScanCodeArrowDown) This->CurrentOption++;
        else if (Key->ScanCode == ScanCodeArrowUp) This->CurrentOption--;

        // Check current selection bounds
        if (This->CurrentOption < 0)
            This->CurrentOption = 0;
        else if (This->CurrentOption > EfiScreenInfoMenuN - 1)
            This->CurrentOption = EfiScreenInfoMenuN - 1;
    }

    return EfiScreenInfoMenuState;
}

VOID ScreenInfoMenuUpdate(EFI_MENU_PAGE *This)
{
    CHAR16 *Options[EfiScreenInfoMenuN] = {
        [EfiScreenInfoMenuSetTextMode] = u"Set Text Mode",
        [EfiScreenInfoMenuBack]        = u"Back to Main Menu",
    };
    INT32 TopSelectableRow = cOut->Mode->CursorRow;
    if (This->RedrawNeeded)
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
            cOut->Mode->Mode+1,
            cOut->Mode->MaxMode,
            cOut->Mode->CursorRow,
            cOut->Mode->CursorColumn,
            cOut->Mode->CursorVisible,
            cols, rows
        );

        CHAR16 Suffix[16];
        for (INT32 i=0; i<cOut->Mode->MaxMode; i++)
        {
            Suffix[0] = '\0'; // Reset String to null
            if (i == cOut->Mode->Mode)
                StrCpySafe(Suffix, u" (selected)");

            cOut->QueryMode(cOut, i, &cols, &rows);
            Printf(u"Mode %d: %d x %d%s\r\n", i+1, cols, rows, Suffix);
        }
        Printf(u"\n");
        TopSelectableRow = cOut->Mode->CursorRow;

        for (INT32 i=0; i < EfiScreenInfoMenuN; i++)
        {
            cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
            Suffix[0] = '\0'; // Reset String to null
            if (i == This->CurrentOption) {
                cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
                if (This->AwaitingInput)
                    sPrintfSafe(Suffix, u" (1-%d): %s", cOut->Mode->MaxMode, This->InputBuffer);
            }

            Printf(u"%s%s\r\n", Options[i], Suffix);
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

// Initialize ScreenInfoMenu Page
EFI_MENU_PAGE *ScreenInfoMenu(VOID)
{
    EFI_MENU_PAGE *ScreenInfoMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID**)&ScreenInfoMenuPtr);
    *ScreenInfoMenuPtr = DefaultPage;
    ScreenInfoMenuPtr->ProcessInput = ScreenInfoMenuProcessInput;
    ScreenInfoMenuPtr->Update = ScreenInfoMenuUpdate;

    CHAR16 tmp[2];
    IntToStr(tmp, cOut->Mode->Mode + 1);
    StrlCpy(ScreenInfoMenuPtr->InputBuffer, tmp, ARRAY_LEN(ScreenInfoMenuPtr->InputBuffer));
    return ScreenInfoMenuPtr;
}
