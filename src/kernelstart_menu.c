#include "menu.h"

EFI_STATUS KernelStart(VOID)
{
    return EFI_SUCCESS;
}

EFI_MENU_STATE KernelStartMenuProcessInput(EFI_MENU_PAGE *Base, EFI_INPUT_KEY *Key)
{
    (VOID)Base;

    if (Key->ScanCode == ScanCodeEscape)
        return EfiMainMenuState;

    return EfiKernelStartState;
}

VOID KernelStartMenuUpdate(EFI_MENU_PAGE *Base)
{
    (VOID)Base;

    Status = KernelStart();
    if (EFI_ERROR(Status))
        Printf(u"ERROR STARTING KERNEL\r\n");
    else
        Printf(u"SUCCESSFUL RETURN FROM KERNEL\r\n");
}

// Initialize KernelStartMenu Page
EFI_MENU_PAGE *KernelStartMenu(VOID)
{
    // Allocate space for the screen info page
    EFI_MENU_PAGE *KernelStartMenuPtr;
    BS->AllocatePool(EfiLoaderData, sizeof(EFI_MENU_PAGE), (VOID **)&KernelStartMenuPtr);

    // Set up defaults
    *KernelStartMenuPtr = DefaultPage;
    KernelStartMenuPtr->ProcessInput = KernelStartMenuProcessInput;
    KernelStartMenuPtr->Update = KernelStartMenuUpdate;

    return (EFI_MENU_PAGE *)KernelStartMenuPtr;
}
