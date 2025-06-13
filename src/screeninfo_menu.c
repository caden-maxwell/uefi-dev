#include "menu.h"

typedef enum EFI_SCREEN_INFO_MENU_OPTIONS
{
    EfiScreenInfoMenuSetTextMode,
    EfiScreenInfoMenuBack,
    EfiScreenInfoMenuN
} EFI_SCREEN_INFO_MENU_OPTIONS;

EFI_MENU_STATE ScreenInfoMenuProcessInput(EFI_MENU_PAGE *Base, EFI_INPUT_KEY *Key)
{
    EFI_INFO_PAGE *Self = (EFI_INFO_PAGE *)Base;
    // Printf(u"KEY: %c, SCANCODE: 0x%x\r\n", Key->UnicodeChar, Key->ScanCode); // For debugging

    // Entering mode number
    if (Base->AwaitingInput)
    {
        // Get the current and max modes
        INT16 PrevMode = cOut->Mode->Mode;
        INT16 NewMode = PrevMode;
        INT16 MaxMode = cOut->Mode->MaxMode - 1;
        INT16 NewNum = Key->UnicodeChar - '0';

        // If [Enter] or [ESC], stop getting input
        if (Key->UnicodeChar == UnicodeCharNewline || Key->ScanCode == ScanCodeEscape)
        {
            Base->RedrawNeeded = TRUE;
            Base->AwaitingInput = FALSE;
        }
        // If [0-9], add to buffer if resulting num is in range
        else if (0 <= NewNum && NewNum <= 9)
        {
            NewMode = PrevMode*10 + NewNum;
            // If resulting mode is invalid, just reset the buffer to the new char
            if (NewMode > MaxMode)
                NewMode = NewNum;
        }
        // If [Down], increment the mode
        else if (Key->ScanCode == ScanCodeArrowDown)
            NewMode++;
        // If [Up], decrement the mode
        else if (Key->ScanCode == ScanCodeArrowUp)
            NewMode--;
        // If [Backspace], remove one char from buffer
        else if (Key->UnicodeChar == UnicodeCharBackspace)
            NewMode = PrevMode/10;

        // Clamp values to [0-Max]
        if (NewMode > MaxMode) NewMode = MaxMode;
        if (NewMode < 0) NewMode = 0;
        Self->CurrentMode = NewMode;

        // If the mode changed, trigger screen update
        if (NewMode != PrevMode)
            Self->DoSetMode = TRUE;
    }
    else // Select submenu/perform action
    {
        if (Key->UnicodeChar == UnicodeCharNewline)
        {
            switch (Base->CurrentOption)
            {
            case EfiScreenInfoMenuSetTextMode:
            {
                Base->AwaitingInput = TRUE;
                Base->RedrawNeeded = TRUE;
                break;
            }
            case EfiScreenInfoMenuBack:
                return EfiMainMenuState;
            }
        }
        else if (Key->ScanCode == ScanCodeEscape)
            return EfiMainMenuState;

        Base->PrevOption = Base->CurrentOption;

        // Move selection
        if (Key->ScanCode == ScanCodeArrowDown)
            Base->CurrentOption++;
        else if (Key->ScanCode == ScanCodeArrowUp)
            Base->CurrentOption--;

        // Check current selection bounds
        if (Base->CurrentOption < 0)
            Base->CurrentOption = 0;
        else if (Base->CurrentOption > EfiScreenInfoMenuN - 1)
            Base->CurrentOption = EfiScreenInfoMenuN - 1;
    }

    return EfiScreenInfoMenuState;
}

VOID ScreenInfoMenuUpdate(EFI_MENU_PAGE *Base)
{
    EFI_INFO_PAGE *Self = (EFI_INFO_PAGE *)Base;
    CHAR16 *OptionLabels[EfiScreenInfoMenuN] = {
        [EfiScreenInfoMenuSetTextMode] = u"Set Text Mode",
        [EfiScreenInfoMenuBack]        = u"Back to Main Menu",
    };

    if (Self->DoSetMode)
    {
        cOut->SetMode(cOut, Self->CurrentMode);
        Self->DoSetMode = FALSE;
        Base->RedrawNeeded = TRUE;
    }

    INT32 TopSelectableRow = cOut->Mode->CursorRow;
    if (Base->RedrawNeeded)
    {
        Base->RedrawNeeded = FALSE;
        cOut->ClearScreen(cOut);

        UINTN cols = 0;
        UINTN rows = 0;
        cOut->QueryMode(cOut, cOut->Mode->Mode, &cols, &rows);
        Printf(
            u"===== Screen Info =====\r\n\n"
            u"Current Mode: %d\r\n\n"
            u"MaxMode: %d\r\n"
            u"CursorVisible: %d\r\n"
            u"Columns: %d\r\n"
            u"Rows: %d\r\n\n",
            cOut->Mode->Mode,
            cOut->Mode->MaxMode,
            cOut->Mode->CursorVisible,
            cols, rows);

        CHAR16 Suffix[16];
        for (INT32 i = 0; i < cOut->Mode->MaxMode; i++)
        {
            Suffix[0] = '\0'; // Reset String to null
            if (i == cOut->Mode->Mode)
                StrCpySafe(Suffix, u" (selected)");

            cOut->QueryMode(cOut, i, &cols, &rows);
            Printf(u"Mode %d: %d x %d%s\r\n", i, cols, rows, Suffix);
        }
        Printf(u"\n");
        TopSelectableRow = cOut->Mode->CursorRow;

        for (INT32 i = 0; i < EfiScreenInfoMenuN; i++)
        {
            cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
            Suffix[0] = '\0'; // Reset String to null
            if (i == Base->CurrentOption)
            {
                cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
                if (Base->AwaitingInput)
                    sPrintfSafe(Suffix, u" (0-%d): %d", cOut->Mode->MaxMode - 1, cOut->Mode->Mode);
            }

            Printf(u"%s%s\r\n", OptionLabels[i], Suffix);
        }
        cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

        cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
        return;
    }

    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + Base->PrevOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    Printf(u"%s\r", OptionLabels[Base->PrevOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + Base->CurrentOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
    Printf(u"%s\r", OptionLabels[Base->CurrentOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
}

// Initialize ScreenInfoMenu Page
EFI_MENU_PAGE *ScreenInfoMenu(VOID)
{
    // Allocate space for the screen info page
    EFI_INFO_PAGE *ScreenInfoMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_INFO_PAGE), (VOID **)&ScreenInfoMenuPtr);

    // Set up defaults
    ScreenInfoMenuPtr->Base = DefaultPage;
    ScreenInfoMenuPtr->Base.ProcessInput = ScreenInfoMenuProcessInput;
    ScreenInfoMenuPtr->Base.Update = ScreenInfoMenuUpdate;
    ScreenInfoMenuPtr->CurrentMode = cOut->Mode->Mode;
    ScreenInfoMenuPtr->DoSetMode = FALSE;

    return (EFI_MENU_PAGE *)ScreenInfoMenuPtr;
}
