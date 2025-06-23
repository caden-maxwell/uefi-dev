#include "menu.h"

EFI_GUID FileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID LoadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID;

EFI_STATUS GetFileInfo(IN EFI_FILE_PROTOCOL *File, OUT EFI_FILE_INFO *FileInfo, OUT UINTN *InfoSize)
{
    UINTN BufSize = 0;
    
    // First call only to get the size of the buffer needed
    Status = File->GetInfo(File, &FileInfoGuid, &BufSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL)
        return Status;
    *InfoSize = BufSize;

    // Second call to actually get file info
    return File->GetInfo(File, &FileInfoGuid, InfoSize, FileInfo);
}

EFI_STATUS OpenNewFile(IN EFI_FILE_PROTOCOL *Dir, OUT EFI_FILE_PROTOCOL **NewFile, IN CHAR16 *Path, IN UINT64 Attrs)
{
    return Dir->Open(
        Dir,
		NewFile,
		Path,
        EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
        Attrs
    );
}

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

    BS->CloseProtocol(LoadedImage->DeviceHandle, &FileSystemGuid, Image, NULL);
    BS->CloseProtocol(Image, &LoadedImageGuid, Image, NULL);

    // Print all files in root directory
    EFI_FILE_INFO *FileInfo;
    UINTN BufSize;
    RootDir->SetPosition(RootDir, 0);
    Printf(u"FILES:\r\n");
    while (TRUE) {
        // Read to get size of fileinfo needed
        BufSize = 0;
        Status = RootDir->Read(RootDir, &BufSize, NULL);
        if (Status != EFI_BUFFER_TOO_SMALL && Status != EFI_SUCCESS) return Status;
        if (BufSize == 0) break;

        // Allocate some memory for fileinfo
        Status = BS->AllocatePool(EfiLoaderData, BufSize, (VOID **)&FileInfo);
        if (EFI_ERROR(Status))
        {
            PrintfErr(u"Could not allocate memory for FileInfo!\r\n");
            break;
        }

        // Read again with correct size to actually get file info
        Status = RootDir->Read(RootDir, &BufSize, FileInfo);
        if (!EFI_ERROR(Status))
        {
            EFI_FILE_PROTOCOL *File;
            Status = RootDir->Open(RootDir, &File, FileInfo->FileName, EFI_FILE_MODE_READ, NULL);
            if (EFI_ERROR(Status))
            {
                PrintfErr(u"Failed to open file '%s': ERROR CODE 0x%x\r\n", FileInfo->FileName, Status);
                continue;
            }

            Printf(
                u"0x%x %d/%d/%d %d:%d:%d %d %s\r\n",
                FileInfo->Attribute,
                FileInfo->CreateTime.Month,
                FileInfo->CreateTime.Day,
                FileInfo->CreateTime.Year,
                FileInfo->CreateTime.Hour,
                FileInfo->CreateTime.Minute,
                FileInfo->CreateTime.Second,
                FileInfo->FileSize,
                FileInfo->FileName
            );

            // Set archive bit to 0
            FileInfo->Attribute &= ~EFI_FILE_ARCHIVE;
            Status = File->SetInfo(File, &FileInfoGuid, BufSize, FileInfo);
            if (EFI_ERROR(Status))
            {
                PrintfErr(u"Failed to set archive bit for file '%s': ERROR CODE 0x%x\r\n", FileInfo->FileName, Status);
                continue;
            }

            File->Flush(File);
            File->Close(File);
        }

        // Free space before trying to get the next file(s) or returning
        BS->FreePool(FileInfo);
    }

    EFI_FILE_PROTOCOL *NewFile;
    CHAR16 *Path = u"NEWDIR";
    Status = OpenNewFile(RootDir, &NewFile, Path, EFI_FILE_DIRECTORY);
    if (EFI_ERROR(Status))
    {
        Printf(u"Failed to create new file: ERROR CODE 0x%x\r\n", Status);
        return Status;
    }

    NewFile->Flush(NewFile);
    NewFile->Close(NewFile);
    RootDir->Flush(RootDir);
    RootDir->Close(RootDir);

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
