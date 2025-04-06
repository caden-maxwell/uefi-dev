#include <stdint.h>

#ifndef _EFI_H
#define _EFI_H

#define EFIAPI __attribute__((__ms_abi__))
#define PACKED __attribute__((__packed__))
#define ALIGNED4 __attribute__((__aligned__(4)))
#define FALSE (BOOLEAN)(0==1)
#define TRUE  (BOOLEAN)(0==0)

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
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING)(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16                          *String
);

// UEFI Spec 12.4.8
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
);

// UEFI Spec 12.4.1
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET               Reset;
    EFI_TEXT_STRING              OutputString;
    void                         *TestString;// EFI_TEXT_TEST_STRING         TestString;
    void                         *QueryMode;// EFI_TEXT_QUERY_MODE          QueryMode;
    void                         *SetMode;// EFI_TEXT_SET_MODE            SetMode;
    void                         *SetAttribute;// EFI_TEXT_SET_ATTRIBUTE       SetAttribute;
    EFI_TEXT_CLEAR_SCREEN        ClearScreen;
    void                         *SetCursorPosition;// EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    void                         *EnableCursor;// EFI_TEXT_ENABLE_CURSOR       EnableCursor;
    void                         *Mode;// SIMPLE_TEXT_OUTPUT_MODE      *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// ----------------------------------
// ------ EFI_RUNTIME_SERVICES ------
// ----------------------------------

typedef struct _EFI_RUNTIME_SERVICES {

} EFI_RUNTIME_SERVICES;

// -------------------------------
// ------ EFI_BOOT_SERVICES ------
// -------------------------------

typedef struct _EFI_BOOT_SERVICES {

} EFI_BOOT_SERVICES;

// -------------------------------------
// ------ EFI_CONFIGURATION_TABLE ------
// -------------------------------------

typedef struct _EFI_CONFIGURATION_TABLE {

} EFI_CONFIGURATION_TABLE;

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
