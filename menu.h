#include "efilib.h"

#define MENU_ARROW_UP   1
#define MENU_ARROW_DOWN 2
#define MENU_NEWLINE    0xD

typedef struct _EFI_MENU_PAGE EFI_MENU_PAGE;

typedef enum _EFI_MENU_STATE {
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

// ===== Abstract Menu Page =====

typedef struct _EFI_MENU_PAGE {
    EFI_MENU_PROCESS_INPUT ProcessInput;
    EFI_MENU_UPDATE        Update;
} EFI_MENU_PAGE;

// ===== Main Menu =====

typedef struct _EFI_MAIN_MENU {
    EFI_MENU_PAGE Page;
    INT32        CurrentOption;
} EFI_MAIN_MENU;

EFI_MAIN_MENU *MainMenu(VOID);

// ===== Some other Menu =====

typedef struct _EFI_OTHER_MENU {
    EFI_MENU_PAGE Page;
    INT32        Something;
} EFI_OTHER_MENU;

EFI_OTHER_MENU *OtherMenu(VOID);
