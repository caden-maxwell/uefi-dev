#include "menu.h"

typedef enum EFI_GOP_INFO_MENU_OPTIONS
{
    EfiGOPInfoMenuSetTextMode,
    EfiGOPInfoMenuBack,
    EfiGOPInfoMenuN
} EFI_GOP_INFO_MENU_OPTIONS;

EFI_MENU_STATE GOPInfoMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    // Printf(u"KEY: %c, SCANCODE: 0x%x\r\n", Key->UnicodeChar, Key->ScanCode); // For debugging

    // Entering mode number
    if (This->AwaitingInput)
    {
        // Get the current and max modes
        INTN PrevMode = GOP->Mode->Mode;
        INTN NewMode = PrevMode;
        INTN MaxMode = GOP->Mode->MaxMode - 1;

        // If [Enter] or [ESC], stop getting input
        if (Key->UnicodeChar == UnicodeCharNewline || Key->ScanCode == ScanCodeEscape)
        {
            This->RedrawNeeded = TRUE; // Need to redraw to get rid of suffixes
            This->AwaitingInput = FALSE;
        }
        // If [Down], increment the mode
        else if (Key->ScanCode == ScanCodeArrowDown)
        {
            if (++NewMode > MaxMode)
                NewMode = MaxMode;
        }
        // If [Up], decrement the mode
        else if (Key->ScanCode == ScanCodeArrowUp)
        {
            if (--NewMode < 0)
                NewMode = 0;
        }
        // If [1-MaxMode], switch to mode
        // TODO: Can only handle a single char in the buffer. Need to handle 2+
        //  With my current device, there are 30 GOP modes
        else if (Key->UnicodeChar > '0' && Key->UnicodeChar - '0' - 1 <= MaxMode)
        {
            NewMode = Key->UnicodeChar - '0' - 1;
        }

        // If the mode changed, set it and update the screen
        if (NewMode != PrevMode)
        {
            Status = GOP->SetMode(GOP, NewMode);
            if (Status == EFI_UNSUPPORTED || Status == EFI_DEVICE_ERROR) {
                NewMode = PrevMode;
                GOP->SetMode(GOP, NewMode);
            }
            IntToStr(This->InputBuffer, ++NewMode);
            This->RedrawNeeded = TRUE;
        }

        // Select submenu/perform action
    }
    else
    {
        if (Key->UnicodeChar == UnicodeCharNewline)
        {
            switch (This->CurrentOption)
            {
            case EfiGOPInfoMenuSetTextMode:
            {
                This->AwaitingInput = TRUE;
                This->RedrawNeeded = TRUE;
                break;
            }
            case EfiGOPInfoMenuBack:
                return EfiMainMenuState;
            }
        }
        else if (Key->ScanCode == ScanCodeEscape)
            return EfiMainMenuState;

        This->PrevOption = This->CurrentOption;

        // Move selection
        if (Key->ScanCode == ScanCodeArrowDown)
            This->CurrentOption++;
        else if (Key->ScanCode == ScanCodeArrowUp)
            This->CurrentOption--;

        // Check current selection bounds
        if (This->CurrentOption < 0)
            This->CurrentOption = 0;
        else if (This->CurrentOption > EfiGOPInfoMenuN - 1)
            This->CurrentOption = EfiGOPInfoMenuN - 1;
    }

    return EfiGOPInfoMenuState;
}

VOID GOPInfoMenuUpdate(EFI_MENU_PAGE *This)
{
    CHAR16 *OptionLabels[EfiGOPInfoMenuN] = {
        [EfiGOPInfoMenuSetTextMode] = u"Set GOP Mode",
        [EfiGOPInfoMenuBack]        = u"Back to Main Menu",
    };

    INT32 TopSelectableRow = cOut->Mode->CursorRow;
    if (This->RedrawNeeded)
    {
        This->RedrawNeeded = FALSE;
        cOut->ClearScreen(cOut);
        This->DebugInfo.NumDraws++;

        UINTN cols = 0;
        UINTN rows = 0;
        UINTN InfoSize;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GOPInfo;
        GOP->QueryMode(GOP, GOP->Mode->Mode, &InfoSize, &GOPInfo);
        Printf(
            u"===== GOP Info =====\r\n\n"
            u"Current Mode: %d\r\n\n"
            u"MaxMode: %d\r\n"
            u"Resolution: %dx%d\r\n"
            u"BGRR: %x %x %x %x\r\n\n",
            GOP->Mode->Mode + 1,
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
            if (i == This->CurrentOption)
            {
                cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
                if (This->AwaitingInput)
                    sPrintfSafe(Suffix, u" (1-%d): %s", GOP->Mode->MaxMode, This->InputBuffer);
            }

            Printf(u"%s%s\r\n", OptionLabels[i], Suffix);
        }
        cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));

        cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
        return;
    }

    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + This->PrevOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
    Printf(u"%s\r", OptionLabels[This->PrevOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow + This->CurrentOption);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE));
    Printf(u"%s\r", OptionLabels[This->CurrentOption]);
    cOut->SetCursorPosition(cOut, 0, TopSelectableRow);
    cOut->SetAttribute(cOut, EFI_TEXT_ATTR(EFI_BLUE, EFI_LIGHTGRAY));
}

// Initialize GOPInfoMenu Page
EFI_MENU_PAGE *GOPInfoMenu(VOID)
{
    EFI_MENU_PAGE *GOPInfoMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID **)&GOPInfoMenuPtr);
    *GOPInfoMenuPtr = DefaultPage;
    GOPInfoMenuPtr->ProcessInput = GOPInfoMenuProcessInput;
    GOPInfoMenuPtr->Update = GOPInfoMenuUpdate;

    CHAR16 tmp[3];
    IntToStr(tmp, GOP->Mode->Mode + 1);
    StrCpySafe(GOPInfoMenuPtr->InputBuffer, tmp);
    return GOPInfoMenuPtr;
}
