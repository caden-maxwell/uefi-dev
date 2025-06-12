#include "menu.h"

typedef enum EFI_GOP_INFO_MENU_OPTIONS
{
    EfiGOPInfoMenuSetTextMode,
    EfiGOPInfoMenuBack,
    EfiGOPInfoMenuN
} EFI_GOP_INFO_MENU_OPTIONS;

EFI_MENU_STATE GOPInfoMenuProcessInput(EFI_MENU_PAGE *Base, EFI_INPUT_KEY *Key)
{
    EFI_GOP_INFO_MENU_PAGE *Self = (EFI_GOP_INFO_MENU_PAGE *)Base;
    // Printf(u"KEY: %c, SCANCODE: 0x%x\r\n", Key->UnicodeChar, Key->ScanCode); // For debugging

    // Entering mode number
    if (Base->AwaitingInput)
    {
        // Get the current and max modes
        INT16 PrevMode = GOP->Mode->Mode;
        INT16 NewMode = PrevMode;
        INT16 MaxMode = GOP->Mode->MaxMode - 1;
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

        // If the mode changed, set it and update the screen
        if (NewMode != PrevMode)
        {
            Self->CurrentMode = NewMode;
            Base->RedrawNeeded = TRUE;
            Self->DoSetMode = TRUE;
        }
    }
    else // Select submenu/perform action
    {
        if (Key->UnicodeChar == UnicodeCharNewline)
        {
            switch (Base->CurrentOption)
            {
            case EfiGOPInfoMenuSetTextMode:
            {
                Base->AwaitingInput = TRUE;
                Base->RedrawNeeded = TRUE;
                break;
            }
            case EfiGOPInfoMenuBack:
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
        else if (Base->CurrentOption > EfiGOPInfoMenuN - 1)
            Base->CurrentOption = EfiGOPInfoMenuN - 1;
    }

    return EfiGOPInfoMenuState;
}

VOID GOPInfoMenuUpdate(EFI_MENU_PAGE *Base)
{
    CHAR16 *OptionLabels[EfiGOPInfoMenuN] = {
        [EfiGOPInfoMenuSetTextMode] = u"Set GOP Mode",
        [EfiGOPInfoMenuBack]        = u"Back to Main Menu",
    };

    EFI_GOP_INFO_MENU_PAGE *Self = (EFI_GOP_INFO_MENU_PAGE *)Base;

    if (Self->DoSetMode)
    {
        Self->DoSetMode = FALSE;
        IntToStr(Base->InputBuffer, Self->CurrentMode);
        GOP->SetMode(GOP, Self->CurrentMode);
    }

    INT32 TopSelectableRow = cOut->Mode->CursorRow;
    if (Base->RedrawNeeded)
    {
        Base->RedrawNeeded = FALSE;
        cOut->ClearScreen(cOut);

        UINTN InfoSize;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GOPInfo;
        GOP->QueryMode(GOP, GOP->Mode->Mode, &InfoSize, &GOPInfo);
        Printf(
            u"===== GOP Info =====\r\n\n"
            u"Current Mode: %d\r\n\n"
            u"MaxMode: %d\r\n"
            u"Resolution: %dx%d\r\n"
            u"BGRR: %x %x %x %x\r\n\n",
            GOP->Mode->Mode,
            GOP->Mode->MaxMode,
            GOPInfo->HorizontalResolution,
            GOPInfo->VerticalResolution,
            GOPInfo->PixelInformation.BlueMask,
            GOPInfo->PixelInformation.GreenMask,
            GOPInfo->PixelInformation.RedMask,
            GOPInfo->PixelInformation.ReservedMask
        );
        TopSelectableRow = cOut->Mode->CursorRow;

        CHAR16 Suffix[16];
        for (INT32 i = 0; i < EfiGOPInfoMenuN; i++)
        {
            cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
            Suffix[0] = '\0'; // Reset String to null
            if (i == Base->CurrentOption)
            {
                cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
                if (Base->AwaitingInput)
                    sPrintfSafe(Suffix, u" (0-%d): %s", GOP->Mode->MaxMode-1, Base->InputBuffer);
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

// Initialize GOPInfoMenu Page
EFI_MENU_PAGE *GOPInfoMenu(VOID)
{
    EFI_GOP_INFO_MENU_PAGE *GOPInfoMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_GOP_INFO_MENU_PAGE), (VOID **)&GOPInfoMenuPtr);
    GOPInfoMenuPtr->Base = DefaultPage;
    GOPInfoMenuPtr->Base.ProcessInput = GOPInfoMenuProcessInput;
    GOPInfoMenuPtr->Base.Update = GOPInfoMenuUpdate;

    CHAR16 tmp[3];
    IntToStr(tmp, GOP->Mode->Mode);
    StrCpySafe(GOPInfoMenuPtr->Base.InputBuffer, tmp);
    return (EFI_MENU_PAGE *)GOPInfoMenuPtr;
}
