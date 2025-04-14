#include "efilib.h"

typedef enum SCAN_CODE {
    ScanCodeArrowUp   = 1,
    ScanCodeArrowDown = 2,
} SCAN_CODE;

typedef enum UNICODE_CHAR {
    UnicodeCharNewline = 0xD,
} UNICODE_CHAR;

typedef struct EFI_MENU_PAGE EFI_MENU_PAGE;

typedef enum EFI_MENU_STATE {
    EfiMainMenuState,
    EfiOtherMenuState,
    EfiExitState
} EFI_MENU_STATE;

// ===== Generic Menu functions =====

typedef
EFI_MENU_STATE
(*EFI_MENU_PROCESS_INPUT)(
    IN EFI_MENU_PAGE *This,
    IN EFI_INPUT_KEY *Key
);

typedef
VOID
(*EFI_MENU_UPDATE)(
    IN EFI_MENU_PAGE *This
);

typedef struct EFI_MENU_PAGE {
    EFI_MENU_PROCESS_INPUT ProcessInput;
    EFI_MENU_UPDATE        Update;
    BOOLEAN                InitialRender;
    INT32                  CurrentOption;
    INT32                  PrevOption;
} EFI_MENU_PAGE;

EFI_MENU_PAGE *MainMenu(VOID);
EFI_MENU_PAGE *OtherMenu(VOID);
