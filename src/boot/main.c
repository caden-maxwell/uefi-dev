#include <efi.h>
#include <efilib.h>

EFI_FILE *
LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE *LoadedFile;
	EFI_STATUS Status;

	Print(L"Loading image protocol\n");
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &LoadedImageProtocol, (void **)&LoadedImage);
	if (Status != EFI_SUCCESS)
	{
		Print(L"Could not load image protocol\n");
		Print(L"Error code: %d\n", Status);
		return NULL;
	}
	else
		Print(L"Loaded image protocol\n");

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	Status = BS->HandleProtocol(LoadedImage->DeviceHandle, &FileSystemProtocol, (void **)&FileSystem);
	if (Status != EFI_SUCCESS)
	{
		Print(L"Could not load file system protocol\n");
		Print(L"Error code: %d\n", Status);
		return NULL;
	}
	else
		Print(L"Loaded file system protocol\n");

	if (Directory == NULL)
	{
		Print(L"Opening volume...\n");
		FileSystem->OpenVolume(FileSystem, &Directory);
		Print(L"Opened volume\n");
	}

	Status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (Status != EFI_SUCCESS)
	{
		return NULL;
	}

	return LoadedFile;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);

	Print(L"Loading kernel...\n");
	// if (LoadFile(NULL, L"\\EFI\\BOOT\\kernel.elf", ImageHandle, SystemTable) == NULL)
	// 	Print(L"Could not load the kernel\n");
	// else
	// 	Print(L"Kernel loaded successfully\n");

	return EFI_SUCCESS;
}
