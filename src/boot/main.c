#include <efi.h>
#include <efilib.h>

EFI_FILE *
LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle)
{
	EFI_FILE *LoadedFile;
	EFI_STATUS Status;

	EFI_LOADED_IMAGE *LoadedImage;
	Status = BS->HandleProtocol(ImageHandle, &LoadedImageProtocol, (void **)&LoadedImage);
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
	else
		Print(L"Directory is not null\n");

	Status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (Status != EFI_SUCCESS)
	{
		Print(L"Could not load file\n");
		return NULL;
	}

	return LoadedFile;
}

EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);
	EFI_STATUS Status;

	Print(L"\n--== Welcome to the kernel loader ==--\n");
	Print(L"Loading kernel...\n");
	EFI_FILE *Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle);
	if (Kernel == NULL)
		Print(L"Could not load the kernel\n");
	else
		Print(L"Kernel loaded successfully\n\n");

	Print(L"Press any key to exit...\n");
	EFI_INPUT_KEY Key;
	/* Now wait for a keystroke before continuing, otherwise your
	   message will flash off the screen before you see it.

	   First, we need to empty the console input buffer to flush
	   out any keystrokes entered before this point */
	Status = ST->ConIn->Reset(ST->ConIn, FALSE);
	if (EFI_ERROR(Status))
		return Status;

	/* Now wait until a key becomes available.  This is a simple
	   polling implementation.  You could try and use the WaitForKey
	   event instead if you like */
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);

	return EFI_SUCCESS;
}
