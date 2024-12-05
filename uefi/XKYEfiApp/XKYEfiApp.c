#include "ProcessorBind.h"
#include "Uefi/UefiBaseType.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/BlockIo.h>
#include <Guid/FileInfo.h>

typedef VOID (*KERNEL_ENTRY_POINT)(VOID);

EFI_STATUS
EFIAPI
LoadFileToAddress(
    IN EFI_FILE_PROTOCOL *File,
    IN EFI_PHYSICAL_ADDRESS LoadAddress
    ){

    EFI_STATUS Status;
    EFI_FILE_INFO *FileInfo = NULL;
    UINTN FileInfoSize = 0;
    UINTN BufferSize = 0;
    VOID *FileBuffer = NULL;

    // First, get the size of the EFI_FILE_INFO structure
    FileInfoSize = 0;
    Status = File->GetInfo(
        File,
        &gEfiFileInfoGuid,
        &FileInfoSize,
        NULL
        );
    if (Status != EFI_BUFFER_TOO_SMALL) {
          Print(L"Failed to get file info size: %r\n", Status);
          goto Cleanup;
    }

    // Allocate buffer for EFI_FILE_INFO
    FileInfo = AllocateZeroPool(FileInfoSize);
    if (FileInfo == NULL) {
      Print(L"Failed to allocate memory for file info\n");
      Status = EFI_OUT_OF_RESOURCES;
      goto Cleanup;
    }

    Status = File->GetInfo(
        File,
        &gEfiFileInfoGuid,
        &FileInfoSize,
        FileInfo
        );
    if (EFI_ERROR(Status)) {
      Print(L"Failed to get file info: %r\n", Status);
      goto Cleanup;
    }

    // Allocate buffer to read the file
    BufferSize = (UINTN)FileInfo->FileSize;
    FileBuffer = AllocateZeroPool(BufferSize);
    if (FileBuffer == NULL) {
      Print(L"Failed to allocate memory for file buffer\n");
      Status = EFI_OUT_OF_RESOURCES;
      goto Cleanup;
    }

    // Read the file contents into the buffer
    Status = File->Read(
        File,
        &BufferSize,
        FileBuffer
        );
    if (EFI_ERROR(Status)) {
      Print(L"Failed to read file: %r\n", Status);
      goto Cleanup;
    }

    Print(L"'%s' read successfully, size: %u bytes\n", FileInfo->FileName, BufferSize);

    /* Allocate memory at LoadAddress to place the file contents */
    UINTN Pages = EFI_SIZE_TO_PAGES(BufferSize);

    Status = gBS->AllocatePages(
        AllocateAddress,
        EfiLoaderData,
        Pages,
        &LoadAddress
        );
    if (EFI_ERROR(Status)) {
      Print(L"Failed to allocate pages at 0x%p: %r\n", LoadAddress, Status);
      goto Cleanup;
    }

    Print(L"Memory allocated at 0x%p\n", LoadAddress);

    /* Copy file contents to load address */
    CopyMem((VOID *) (UINTN) LoadAddress, FileBuffer, BufferSize);

    Print(L"'%s' loaded into memory at 0x%p\n", FileInfo->FileName, LoadAddress);

    Status = EFI_SUCCESS;
Cleanup:

    if (FileBuffer != NULL) {
      FreePool(FileBuffer);
    }

    if (FileInfo != NULL) {
      FreePool(FileInfo);
    }

    return Status;
}

EFI_STATUS
EFIAPI
FindFile(
    IN EFI_HANDLE FsHandle,
    IN CHAR16 *FileName,
    OUT EFI_FILE_PROTOCOL **File
    ){

    EFI_STATUS Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    EFI_FILE_PROTOCOL *Root = NULL;

    /* Check if */
    Status = gBS->HandleProtocol(
        FsHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem
        );

    if (EFI_ERROR(Status)) {
      goto Cleanup;
    }

    Status = FileSystem->OpenVolume(FileSystem, &Root);
    if (EFI_ERROR(Status)) {
      goto Cleanup;
    }

    Status = Root->Open(
        Root,
        File,
        FileName,
        EFI_FILE_MODE_READ,
        0
        );
    if (EFI_ERROR(Status)) {
      goto Cleanup;
    }

    Status = EFI_SUCCESS;

Cleanup:

    if (Root != NULL) {
      Root->Close(Root);
    }

    return Status;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    ) {
  EFI_STATUS Status;

  EFI_HANDLE *FsHandles = NULL;
  UINTN FsHandleCount = 0;
  CHAR16* FileName = L"XKY_JETSON_APP.bin";
  EFI_FILE_PROTOCOL *File = NULL;

  Status = gBS->LocateHandleBuffer(
      ByProtocol,
      &gEfiSimpleFileSystemProtocolGuid,
      NULL,
      &FsHandleCount,
      &FsHandles
      );

  if (EFI_ERROR(Status)) {
    Print(L"Failed to locate filesystem handles: %r\n", Status);
    return Status;
  }

  EFI_DEVICE_PATH_PROTOCOL *DevicePath = NULL;

  UINTN Index;
  for(Index = 0; Index < FsHandleCount; Index++){
    Status = gBS->HandleProtocol(
        FsHandles[Index],
        &gEfiDevicePathProtocolGuid,
        (VOID **)&DevicePath
        );

    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = FindFile(
        FsHandles[Index],
        FileName,
        &File
        );
    if(EFI_ERROR(Status)){
        continue;
    }

    Print(L"Found file '%s' at\n", FileName);
    Print(L"  DevicePath: %s\n",
        DevicePath != NULL ? ConvertDevicePathToText(DevicePath, FALSE, FALSE) : L"<Unavailable>");
    break;
  }

  if(File == NULL){
    Print(L"File '%s' not found: %r\n", FileName, Status);
    goto Cleanup;
  }

  EFI_PHYSICAL_ADDRESS LoadAddress = 0x80000000;
  Status = LoadFileToAddress(File, LoadAddress);
  if(EFI_ERROR(Status)){
    Print(L"Failed to load file '%s' to %p\n", FileName, LoadAddress);
    goto Cleanup;
  }

  Print(L"Entering Kernel at address 0x%p\r\n", LoadAddress);


  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
  UINTN MemoryMapSize = 0;
  UINTN MapKey;
  UINTN DescriptorSize;
  UINT32 DescriptorVersion;

  do {
    // Get the size of the memory map
    Status = gBS->GetMemoryMap(
        &MemoryMapSize,
        MemoryMap,
        &MapKey,
        &DescriptorSize,
        &DescriptorVersion
        );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      // Allocate buffer for the memory map
      if (MemoryMap != NULL) {
        FreePool(MemoryMap);
      }
      MemoryMap = AllocatePool(MemoryMapSize);
      if (MemoryMap == NULL) {
        Print(L"Failed to allocate memory for memory map\n");
        Status = EFI_OUT_OF_RESOURCES;
        goto Cleanup;
      }
      // Get the actual memory map
      Status = gBS->GetMemoryMap(
          &MemoryMapSize,
          MemoryMap,
          &MapKey,
          &DescriptorSize,
          &DescriptorVersion
          );
    }
    if (EFI_ERROR(Status)) {
      Print(L"Failed to get memory map: %r\n", Status);
      goto Cleanup;
    }

    Print(L"Exiting boot services...\n");

    Status = gBS->ExitBootServices(
        ImageHandle,
        MapKey
        );

    if (EFI_ERROR(Status)) {
      if (Status == EFI_INVALID_PARAMETER) {
        continue;
      } else {
        Print(L"Failed to exit boot services: %r\n", Status);
        goto Cleanup;
      }
    } else {
      break;
    }
  } while (TRUE);

  // Boot services are now terminated
  // Transfer control to the code at 0x80000000

  KERNEL_ENTRY_POINT KernelEntryPoint = (KERNEL_ENTRY_POINT)(UINTN)LoadAddress;

  /* move to kernel entrypoint */
  KernelEntryPoint();

  /* Execution will never reach here */
  while(1);

Cleanup:


  if (File != NULL) {
    File->Close(File);
  }

  if (FsHandles != NULL) {
    FreePool(FsHandles);
  }

  Print(L"Exiting...\n");

  return EFI_SUCCESS;
}

