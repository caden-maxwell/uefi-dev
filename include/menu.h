#include "efilib.h"

#ifndef _MENU_H
#define _MENU_H

typedef enum SCAN_CODE
{
    ScanCodeArrowUp = 0x01,
    ScanCodeArrowDown = 0x02,
    ScanCodeEscape = 0x17,
} SCAN_CODE;

typedef enum UNICODE_CHAR
{
    UnicodeCharNewline   = 0xD,
    UnicodeCharBackspace = 0x8
} UNICODE_CHAR;

typedef struct EFI_MENU_PAGE EFI_MENU_PAGE;

typedef enum EFI_MENU_STATE
{
    EfiMainMenuState,
    EfiScreenInfoMenuState,
    EfiKernelStartState,
    EfiGOPInfoMenuState,
    EfiExitState
} EFI_MENU_STATE;

extern const EFI_MENU_PAGE DefaultPage;

// ===== Generic Menu functions =====

typedef EFI_MENU_STATE (*EFI_MENU_PROCESS_INPUT)(
    IN EFI_MENU_PAGE *This,
    IN EFI_INPUT_KEY *Key);

typedef VOID (*EFI_MENU_UPDATE)(
    IN EFI_MENU_PAGE *This);

typedef struct EFI_MENU_DEBUG_INFO
{
    INT32 NumDraws;
} EFI_MENU_DEBUG_INFO;

typedef struct EFI_MENU_PAGE
{
    BOOLEAN AwaitingInput;
    BOOLEAN RedrawNeeded;
    INT32 CurrentOption;
    INT32 PrevOption;
    EFI_MENU_DEBUG_INFO DebugInfo;
    EFI_MENU_PROCESS_INPUT ProcessInput;
    EFI_MENU_UPDATE Update;
} EFI_MENU_PAGE;

typedef struct EFI_INFO_PAGE
{
    EFI_MENU_PAGE Base;
    UINT16 CurrentMode;
    BOOLEAN DoSetMode;
} EFI_INFO_PAGE;

EFI_MENU_PAGE *MainMenu(VOID);
EFI_MENU_PAGE *ScreenInfoMenu(VOID);
EFI_MENU_PAGE *GOPInfoMenu(VOID);

#endif // _MENU_H
