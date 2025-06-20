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
            i,
            i*DescSize,
            Desc->Type,
            Desc->PhysicalStart,
            Desc->VirtualStart,
            Desc->NumberOfPages,
            Desc->Attribute
        );
        BS->WaitForEvent(1, &cIn->WaitForKey, NULL);
        
        EFI_INPUT_KEY Key;
        cIn->ReadKeyStroke(cIn, &Key);

        if (Key.ScanCode == ScanCodeEscape)
            break;
    }

    EFI_GUID LoadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage = NULL;
    Status = BS->OpenProtocol(
        Image,
		&LoadedImageGuid,
		(VOID **)&LoadedImage,
		Image,
		NULL,
		EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) return Status;

    EFI_GUID FileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    Status = BS->OpenProtocol(
        LoadedImage->DeviceHandle,
        &FileSystemGuid,
        (VOID **)&FileSystem,
        Image,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) return Status;

    EFI_FILE_PROTOCOL *RootDir;
    Status = FileSystem->OpenVolume(FileSystem, &RootDir);
    if (EFI_ERROR(Status))
    {
        Printf(u"Failed to open volume: ERROR CODE 0x%x\r\n", Status);
        return Status;
    }

    UINTN BufSize = 1024;
    CHAR16 *TmpBuf;
    Status = RootDir->Read(RootDir, &BufSize, (VOID *)TmpBuf);
    if (EFI_ERROR(Status))
    {
        Printf(u"Failed to read root: ERROR CODE 0x%x\r\n", Status);
        return Status;
    }
    EFI_FILE_INFO *Info = (EFI_FILE_INFO *)TmpBuf;
    Printf(
        u"Attribute: 0x%x\r\n"
        u"CreateTime: %d\r\n"
        u"FileName: %s\r\n"
        u"FileSize: %d\r\n"
        u"LastAccessTime: %d\r\n"
        u"ModificationTime: %d\r\n"
        u"PhysicalSize: %d\r\n"
        u"Size: %d\r\n\n",
        Info->Attribute,
        Info->CreateTime,
        Info->FileName,
        Info->FileSize,
        Info->LastAccessTime,
        Info->ModificationTime,
        Info->PhysicalSize,
        Info->Size
    );

    EFI_FILE_PROTOCOL *NewFile = NULL;
    CHAR16 *path = u"NEWFILE";
    Status = RootDir->Open(
        RootDir,
		&NewFile,
		path,
		EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
		NULL
    );
    if (EFI_ERROR(Status))
    {
        Printf(u"Failed to create new file: ERROR CODE 0x%x\r\n", Status);
        return Status;
    }

    BufSize = 1024;
    Status = NewFile->Write(NewFile, &BufSize, (VOID *)u"HELLO, WORLD!");
    if (EFI_ERROR(Status))
    {
        Printf(u"Failed to write to new file: ERROR CODE 0x%x\r\n", Status);
        return Status;
    }

    NewFile->Flush(NewFile);
    NewFile->Close(NewFile);

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
