#include "menu.h"

EFI_STATUS KernelStart(VOID)
{
    UINTN MemMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemMapPtr = NULL;
    UINTN MapKey;
    UINTN DescSize;
    UINT32 DescVersion;
    
    // Get memory map
    BS->GetMemoryMap(&MemMapSize, MemMapPtr, &MapKey, &DescSize, &DescVersion);
    BS->AllocatePool(EfiLoaderData, MemMapSize, (VOID **)&MemMapPtr);
    MemMapSize += DescSize * 2;
    Status = BS->GetMemoryMap(&MemMapSize, MemMapPtr, &MapKey, &DescSize, &DescVersion);

    cOut->ClearScreen(cOut);
    if (Status == EFI_BUFFER_TOO_SMALL)
        Printf(u"ERROR: 0x%x, needs size %u\r\n", Status, MemMapSize);

    if (EFI_ERROR(Status))
    {
        Printf(u"ERROR: 0x%x\r\n", Status);
        while(1);
    }

    for (UINT32 i=0; i<MemMapSize/DescSize; i++)
    {
        cOut->ClearScreen(cOut);
        EFI_MEMORY_DESCRIPTOR *Desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)MemMapPtr + i * DescSize);
        Printf(
            u"Mem Map Size: %u\r\n"
            u"Map Key: %u\r\n"
            u"Descriptor Size: %u\r\n"
            u"Descriptor Version: %u\r\n"
            u"Num Descriptors: %u\r\n\n"
            u"Descriptor Num: %u\r\n"
            u"Current map offset: %d\r\n"
            u"Descriptor Type: %u\r\n"
            u"Descriptor Phys: 0x%x\r\n"
            u"Descriptor Virt: 0x%x\r\n"
            u"Descriptor Num Pages: %u\r\n"
            u"Descriptor Attr: 0x%x\r\n\n",
            MemMapSize,
            MapKey, 
            DescSize,
            DescVersion,
            MemMapSize / DescSize,
            i/DescSize,
            i,
            Desc->Type,
            Desc->PhysicalStart,
            Desc->VirtualStart,
            Desc->NumberOfPages,
            Desc->Attribute
        );
        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        cIn->ReadKeyStroke(cIn, NULL);
    }
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
