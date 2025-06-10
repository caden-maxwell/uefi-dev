#include "menu.h"

const EFI_MENU_PAGE DefaultPage = {
    .RedrawNeeded = TRUE,
    .CurrentOption = 0,
    .PrevOption = 0,
    .ProcessInput = NULL,
    .Update = NULL,
    .AwaitingInput = FALSE,
    .InputBuffer = u"",
    .InputLength = 0,
};
