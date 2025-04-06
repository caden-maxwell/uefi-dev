#include <stdint.h>

#ifndef _EFI_H
#define _EFI_H

#define EFIAPI __attribute__((__ms_abi__))
#define PACKED __attribute__((__packed__))
#define ALIGNED4 __attribute__((__aligned__(4)))
#define FALSE (BOOLEAN)(0==1)
#define TRUE  (BOOLEAN)(0==0)
#define NULL  0

// UEFI Spec 2.3.1
#define IN
#define OUT
#define OPTIONAL
#define CONST const

typedef uint8_t  BOOLEAN;
typedef int64_t  INTN;
typedef uint64_t UINTN;
typedef int8_t   INT8;
typedef uint8_t  UINT8;
typedef int16_t  INT16;
typedef uint16_t UINT16;
typedef int32_t  INT32;
typedef uint32_t UINT32;
typedef int64_t  INT64;
typedef uint64_t UINT64;
typedef uint8_t  CHAR8;
typedef uint16_t char16_t; // Might need uint_least16_t here
typedef char16_t CHAR16;

typedef void   VOID;
typedef UINTN  EFI_STATUS;
typedef VOID  *EFI_HANDLE;
typedef VOID  *EFI_EVENT;
typedef UINT64 EFI_LBA;
typedef UINTN  EFI_TPL;
typedef UINT8  EFI_MAC_ADDRESS[32];
typedef UINT8  EFI_IPv4_ADDRESS[4];
typedef UINT8  EFI_IPv6_ADDRESS[16];
typedef UINT8  ALIGNED4 EFI_IP_ADDRESS[16];

// UEFI Spec. Appendix A
typedef struct {
    UINT32 TimeLow;
    UINT16 TimeMid;
    UINT16 TimeHighAndVersion;
    UINT8  ClockSeqHighAndReserved;
    UINT8  ClockSeqLow;
    UINT8  Node[6];
} PACKED EFI_GUID;

// UEFI Spec. Appendix D
#define HIGH_BIT 0x8000000000000000ULL
#define EFI_SUCCESS              (EFI_STATUS)(0)
#define EFI_ERROR(StatusCode)    (((EFI_STATUS)(StatusCode)) >= HIGH_BIT)

#define ENCODE_ERROR(StatusCode) ((EFI_STATUS)(HIGH_BIT | (StatusCode)))
#define EFI_LOAD_ERROR           ENCODE_ERROR (1)
#define EFI_INVALID_PARAMETER    ENCODE_ERROR (2)
#define EFI_UNSUPPORTED          ENCODE_ERROR (3)
#define EFI_BAD_BUFFER_SIZE      ENCODE_ERROR (4)
#define EFI_BUFFER_TOO_SMALL     ENCODE_ERROR (5)
#define EFI_NOT_READY            ENCODE_ERROR (6)
#define EFI_DEVICE_ERROR         ENCODE_ERROR (7)
#define EFI_WRITE_PROTECTED      ENCODE_ERROR (8)
#define EFI_OUT_OF_RESOURCES     ENCODE_ERROR (9)
#define EFI_VOLUME_CORRUPTED     ENCODE_ERROR (10)
#define EFI_VOLUME_FULL          ENCODE_ERROR (11)
#define EFI_NO_MEDIA             ENCODE_ERROR (12)
#define EFI_MEDIA_CHANGED        ENCODE_ERROR (13)
#define EFI_NOT_FOUND            ENCODE_ERROR (14)
#define EFI_ACCESS_DENIED        ENCODE_ERROR (15)
#define EFI_NO_RESPONSE          ENCODE_ERROR (16)
#define EFI_NO_MAPPING           ENCODE_ERROR (17)
#define EFI_TIMEOUT              ENCODE_ERROR (18)
#define EFI_NOT_STARTED          ENCODE_ERROR (19)
#define EFI_ALREADY_STARTED      ENCODE_ERROR (20)
#define EFI_ABORTED              ENCODE_ERROR (21)
#define EFI_ICMP_ERROR           ENCODE_ERROR (22)
#define EFI_TFTP_ERROR           ENCODE_ERROR (23)
#define EFI_PROTOCOL_ERROR       ENCODE_ERROR (24)
#define EFI_INCOMPATIBLE_VERSION ENCODE_ERROR (25)
#define EFI_SECURITY_VIOLATION   ENCODE_ERROR (26)
#define EFI_CRC_ERROR            ENCODE_ERROR (27)
#define EFI_END_OF_MEDIA         ENCODE_ERROR (28)
#define EFI_END_OF_FILE          ENCODE_ERROR (31)
#define EFI_INVALID_LANGUAGE     ENCODE_ERROR (32)
#define EFI_COMPROMISED_DATA     ENCODE_ERROR (33)
#define EFI_IP_ADDRESS_CONFLICT  ENCODE_ERROR (34)
#define EFI_HTTP_ERROR           ENCODE_ERROR (35)

typedef struct _EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;

// UEFI Spec. 4.1.1
typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_ENTRY_POINT) (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
);

// UEFI Spec 4.2.1
typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

// -------------------------------------------------------------
// ------ EFI_SIMPLE_TEXT_INPUT_PROTOCOL - UEFI Spec 12.3 ------
// -------------------------------------------------------------

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

// UEFI Spec 12.3.2
typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET) (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    IN BOOLEAN                        ExtendedVerification
);

// UEFI Spec 12.3.3
typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY) (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    OUT EFI_INPUT_KEY                 *Key
);

// UEFI Spec 12.3.1
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET    Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT          WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

// --------------------------------------------------------------
// ------ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL - UEFI Spec 12.4 ------
// --------------------------------------------------------------

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// UEFI Spec 12.4.2
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET)(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN BOOLEAN                         ExtendedVerification
);

// UEFI Spec 12.4.3
// TODO: Implement related definitions (boxdraw chars, arrows, shapes, etc.)
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING)(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16                          *String
);

// UEFI Spec 12.4.4
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16                          *String
);

// UEFI Spec 12.4.5
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN                           ModeNumber,
    OUT UINTN                          *Columns,
    OUT UINTN                          *Rows
);

// UEFI Spec 12.4.6
typedef
EFI_STATUS
(* EFIAPI EFI_TEXT_SET_MODE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN                           ModeNumber
);

// UEFI Spec 12.4.7
// TODO: Implement related definitions (colors)
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN                           Attribute
);

// UEFI Spec 12.4.8
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
);

// UEFI Spec 12.4.9
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN                           Column,
    IN UINTN                           Row
);

// UEFI Spec 12.4.10
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN BOOLEAN                         Visible
);

// UEFI Spec 12.4.1
typedef struct {
    INT32   MaxMode;
    INT32   Mode;
    INT32   Attribute;
    INT32   CursorColumn;
    INT32   CursorRow;
    BOOLEAN CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

// UEFI Spec 12.4.1
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET               Reset;
    EFI_TEXT_STRING              OutputString;
    EFI_TEXT_TEST_STRING         TestString;
    EFI_TEXT_QUERY_MODE          QueryMode;
    EFI_TEXT_SET_MODE            SetMode;
    EFI_TEXT_SET_ATTRIBUTE       SetAttribute;
    EFI_TEXT_CLEAR_SCREEN        ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR       EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE      *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// ----------------------------------
// ------ EFI_RUNTIME_SERVICES ------
// ----------------------------------

typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;

// -------------------------------
// ------ EFI_BOOT_SERVICES ------
// -------------------------------

typedef
EFI_STATUS
(EFIAPI *EFI_WAIT_FOR_EVENT) (
    IN UINTN     NumberOfEvents,
    IN EFI_EVENT *Event,
    OUT UINTN    *Index
);

typedef struct {
    EFI_TABLE_HEADER     Hdr;

    //
    // Task Priority Services
    //
    // EFI_RAISE_TPL        RaiseTPL;       // EFI 1.0+
    // EFI_RESTORE_TPL      RestoreTPL;     // EFI 1.0+
    void *RaiseTPL;       // EFI 1.0+
    void *RestoreTPL;     // EFI 1.0+

    //
    // Memory Services
    //
    // EFI_ALLOCATE_PAGES   AllocatePages;  // EFI 1.0+
    // EFI_FREE_PAGES       FreePages;      // EFI 1.0+
    // EFI_GET_MEMORY_MAP   GetMemoryMap;   // EFI 1.0+
    // EFI_ALLOCATE_POOL    AllocatePool;   // EFI 1.0+
    // EFI_FREE_POOL        FreePool;       // EFI 1.0+
    void *AllocatePages;  // EFI 1.0+
    void *FreePages;      // EFI 1.0+
    void *GetMemoryMap;   // EFI 1.0+
    void *AllocatePool;   // EFI 1.0+
    void *FreePool;       // EFI 1.0+

    //
    // Event & Timer Services
    //
    // EFI_CREATE_EVENT     CreateEvent;    // EFI 1.0+
    // EFI_SET_TIMER        SetTimer;       // EFI 1.0+
    // EFI_WAIT_FOR_EVENT   WaitForEvent;   // EFI 1.0+
    // EFI_SIGNAL_EVENT     SignalEvent;    // EFI 1.0+
    // EFI_CLOSE_EVENT      CloseEvent;     // EFI 1.0+
    // EFI_CHECK_EVENT      CheckEvent;     // EFI 1.0+
    void *CreateEvent;    // EFI 1.0+
    void *SetTimer;       // EFI 1.0+
    EFI_WAIT_FOR_EVENT   WaitForEvent;   // EFI 1.0+
    void *SignalEvent;    // EFI 1.0+
    void *CloseEvent;     // EFI 1.0+
    void *CheckEvent;     // EFI 1.0+

    //
    // Protocol Handler Services
    //
    // EFI_INSTALL_PROTOCOL_INTERFACE     InstallProtocolInterface;            // EFI 1.0+
    // EFI_REINSTALL_PROTOCOL_INTERFACE   ReinstallProtocolInterface;          // EFI 1.0+
    // EFI_UNINSTALL_PROTOCOL_INTERFACE   UninstallProtocolInterface;          // EFI 1.0+
    // EFI_HANDLE_PROTOCOL                HandleProtocol;                      // EFI 1.0+
    // VOID*   Reserved;    // EFI 1.0+
    // EFI_REGISTER_PROTOCOL_NOTIFY       RegisterProtocolNotify;              // EFI  1.0+
    // EFI_LOCATE_HANDLE                  LocateHandle;                        // EFI 1.0+
    // EFI_LOCATE_DEVICE_PATH             LocateDevicePath;                    // EFI 1.0+
    // EFI_INSTALL_CONFIGURATION_TABLE       InstallConfigurationTable;           // EFI 1.0+
    void *InstallProtocolInterface;            // EFI 1.0+
    void *ReinstallProtocolInterface;          // EFI 1.0+
    void *UninstallProtocolInterface;          // EFI 1.0+
    void *HandleProtocol;                      // EFI 1.0+
    VOID*   Reserved;    // EFI 1.0+
    void *RegisterProtocolNotify;              // EFI  1.0+
    void *LocateHandle;                        // EFI 1.0+
    void *LocateDevicePath;                    // EFI 1.0+
    void *InstallConfigurationTable;           // EFI 1.0+

    //
    // Image Services
    //
    // EFI_IMAGE_UNLOAD               LoadImage;        // EFI 1.0+
    // EFI_IMAGE_START                StartImage;       // EFI 1.0+
    // EFI_EXIT                       Exit;             // EFI 1.0+
    // EFI_IMAGE_UNLOAD               UnloadImage;      // EFI 1.0+
    // EFI_EXIT_BOOT_SERVICES         ExitBootServices; // EFI 1.0+
    void *LoadImage;        // EFI 1.0+
    void *StartImage;       // EFI 1.0+
    void *Exit;             // EFI 1.0+
    void *UnloadImage;      // EFI 1.0+
    void *ExitBootServices; // EFI 1.0+

    //
    // Miscellaneous Services
    //
    // EFI_GET_NEXT_MONOTONIC_COUNT   GetNextMonotonicCount; // EFI 1.0+
    // EFI_STALL                      Stall;                 // EFI 1.0+
    // EFI_SET_WATCHDOG_TIMER         SetWatchdogTimer;      // EFI 1.0+
    void *GetNextMonotonicCount; // EFI 1.0+
    void *Stall;                 // EFI 1.0+
    void *SetWatchdogTimer;      // EFI 1.0+

    //
    // DriverSupport Services
    //
    // EFI_CONNECT_CONTROLLER         ConnectController;     // EFI 1.1
    // EFI_DISCONNECT_CONTROLLER      DisconnectController;  // EFI 1.1+
    void *ConnectController;     // EFI 1.1
    void *DisconnectController;  // EFI 1.1+

    //
    // Open and Close Protocol Services
    //
    // EFI_OPEN_PROTOCOL              OpenProtocol;           // EFI 1.1+
    // EFI_CLOSE_PROTOCOL             CloseProtocol;          // EFI 1.1+
    // EFI_OPEN_PROTOCOL_INFORMATION     OpenProtocolInformation;// EFI 1.1+
    void *OpenProtocol;           // EFI 1.1+
    void *CloseProtocol;          // EFI 1.1+
    void *OpenProtocolInformation;// EFI 1.1+

    //
    // Library Services
    //
    // EFI_PROTOCOLS_PER_HANDLE       ProtocolsPerHandle;     // EFI 1.1+
    // EFI_LOCATE_HANDLE_BUFFER       LocateHandleBuffer;     // EFI 1.1+
    // EFI_LOCATE_PROTOCOL            LocateProtocol;         // EFI 1.1+
    // EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES  InstallMultipleProtocolInterfaces;    // EFI 1.1+
    // EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;   // EFI 1.1+*
    void *ProtocolsPerHandle;     // EFI 1.1+
    void *LocateHandleBuffer;     // EFI 1.1+
    void *LocateProtocol;         // EFI 1.1+
    void *InstallMultipleProtocolInterfaces;    // EFI 1.1+
    void *UninstallMultipleProtocolInterfaces;   // EFI 1.1+*

    //
    // 32-bit CRC Services
    //
    // EFI_CALCULATE_CRC32    CalculateCrc32;     // EFI 1.1+
    void *CalculateCrc32;     // EFI 1.1+

    //
    // Miscellaneous Services
    //
    // EFI_COPY_MEM           CopyMem;        // EFI 1.1+
    // EFI_SET_MEM            SetMem;         // EFI 1.1+
    // EFI_CREATE_EVENT_EX    CreateEventEx;  // UEFI 2.0+
    void *CopyMem;        // EFI 1.1+
    void *SetMem;         // EFI 1.1+
    void *CreateEventEx;  // UEFI 2.0+
} EFI_BOOT_SERVICES;

// -------------------------------------
// ------ EFI_CONFIGURATION_TABLE ------
// -------------------------------------

typedef struct _EFI_CONFIGURATION_TABLE EFI_CONFIGURATION_TABLE;

// ------------------------------
// ------ EFI_SYSTEM_TABLE ------
// ------------------------------

// UEFI Spec 4.3.1
typedef struct _EFI_SYSTEM_TABLE {
  EFI_TABLE_HEADER                Hdr;
  CHAR16                          *FirmwareVendor;
  UINT32                          FirmwareRevision;
  EFI_HANDLE                      ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn;
  EFI_HANDLE                      ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE                      StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  EFI_RUNTIME_SERVICES            *RuntimeServices;
  EFI_BOOT_SERVICES               *BootServices;
  UINTN                           NumberOfTableEntries;
  EFI_CONFIGURATION_TABLE         *ConfigurationTable;
} EFI_SYSTEM_TABLE;

#endif // _EFI_H
