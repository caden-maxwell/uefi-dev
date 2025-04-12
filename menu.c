#include "efilib.h"
#include "menu.h"

typedef enum _EFI_MAIN_MENU_OPTIONS {
    EfiMainMenuOtherMenu,
    EfiMainMenuExit,
    EfiMainMenuN
} EFI_MAIN_MENU_OPTIONS;

// ====== Main Menu ======

EFI_MENU_STATE MainMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    EFI_MAIN_MENU *MainMenuPage = (EFI_MAIN_MENU*)This;

    // Select submenu
    if (Key->UnicodeChar == 0xD)
    {
        switch (MainMenuPage->CurrentOption) {
            case EfiMainMenuOtherMenu: return EfiOtherMenuState;
            case EfiMainMenuExit: return EfiExitState;
            default: Printf(u"INVALID OPTION!!!");
        }
    }

    // Move selection
    if (Key->ScanCode == MENU_ARROW_DOWN)
        MainMenuPage->CurrentOption++;
    else if (Key->ScanCode == MENU_ARROW_UP)
        MainMenuPage->CurrentOption--;

    // Check current selection bounds
    if (MainMenuPage->CurrentOption < 0)
        MainMenuPage->CurrentOption = 0;
    else if (MainMenuPage->CurrentOption > EfiMainMenuN - 1)
        MainMenuPage->CurrentOption = EfiMainMenuN - 1;

    return EfiMainMenuState;
}

VOID MainMenuUpdate(EFI_MENU_PAGE *This)
{
    EFI_MAIN_MENU *MainMenu = (EFI_MAIN_MENU*)This;

    Printf(u"===== Main Menu =====\r\n\n");

    // Print menu options
    CHAR16 *Options[EfiMainMenuN] = { u"Other Menu", u"Exit" };
    for (UINTN i=0; i < EfiMainMenuN; i++)
        if (MainMenu->CurrentOption == i)
            Printf(u"%s <-\r\n", Options[i]);
        else
            Printf(u"%s\r\n", Options[i]);
}

// Initialize MainMenu Page
EFI_MAIN_MENU *MainMenu(VOID)
{
    EFI_MAIN_MENU *MainMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MAIN_MENU), (VOID**)&MainMenuPtr);
    MainMenuPtr->Page.ProcessInput = MainMenuProcessInput;
    MainMenuPtr->Page.Update = MainMenuUpdate;
    MainMenuPtr->CurrentOption = 0;
    return MainMenuPtr;
}

// ====== Some Other Menu ======

EFI_MENU_STATE OtherMenuProcessInput(EFI_MENU_PAGE *This, EFI_INPUT_KEY *Key)
{
    EFI_OTHER_MENU *OtherMenu = (EFI_OTHER_MENU*)This;
    (void)OtherMenu;
    if (Key->UnicodeChar == 0xD)
        return EfiMainMenuState;

    return EfiOtherMenuState;
}

VOID OtherMenuUpdate(EFI_MENU_PAGE *This)
{
    EFI_OTHER_MENU *MainMenuPage = (EFI_OTHER_MENU*)This;
    (void)MainMenuPage;
    Printf(u"===== Other Stuff =====\r\n\n");

    Printf(u"Current Text Mode: %d\r\n", cOut->Mode->Mode);

    Printf(u"\n");
    Printf(u"Back <-\r\n");
}

// Initialize OtherMenu Page
EFI_OTHER_MENU *OtherMenu(VOID)
{
    EFI_OTHER_MENU *OtherMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_OTHER_MENU), (VOID**)&OtherMenuPtr);
    OtherMenuPtr->Page.ProcessInput = OtherMenuProcessInput;
    OtherMenuPtr->Page.Update = OtherMenuUpdate;
    OtherMenuPtr->Something = 1000;
    return OtherMenuPtr;
}
