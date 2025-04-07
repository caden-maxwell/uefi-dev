#include "efi.h"
#include "efilib.h"
#include <stdarg.h>

EFI_SYSTEM_TABLE  *ST;
EFI_BOOT_SERVICES *BS;
EFI_RUNTIME_SERVICES *RS;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
EFI_STATUS Status;

// =======================================
// ===== Initialize global variables =====
// =======================================
void InitGlobalVars(EFI_SYSTEM_TABLE *SystemTable) {
    ST = SystemTable;
    BS = ST->BootServices;
    RS = ST->RuntimeServices;
    cIn = ST->ConIn;
    cOut = ST->ConOut;
}

// ===================================
// ===== Print formatted strings =====
// ===================================
void Printf(CHAR16 *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    CHAR16 ch;
    for (UINTN i=0; fmt[i] != u'\0'; i++) {
        ch = fmt[i];
        // If we see a format specifier '%'...
        if (ch == u'%') {
            ch = fmt[++i];
            // Check the kind of format specifier
            switch (ch)
            {
                case u's': // '%s' -- string
                {
                    cOut->OutputString(cOut, va_arg(args, CHAR16 *));
                    break;
                }
                case u'c': // '%c' -- character
                {
                    PutChar((CHAR16)va_arg(args, UINTN));
                    break;
                }
                case u'%': // '%%' -- literal %
                {
                    PutChar('%');
                    break;
                }
                case u'd': // '%d' -- integer
                case u'x': // '%x' -- hexadecimal
                {
                    va_arg(args, UINTN); // Unused as of now
                    Printf(u"(Format specifier '%%%c' is not yet implemented)", ch);
                    break;
                }
                default: {
                    va_arg(args, UINTN);
                    Printf(u"(INVALID FORMAT SPECIFIER: '%%%c')", ch);
                }
            }
        }
        // Otherwise, print the char
        else
            PutChar(ch);
    }

    va_end(args);
}

void PutChar(CHAR16 ch) {
    CHAR16 str_arr[2]; // Implement memset so we can shorthand this
    str_arr[0] = ch;
    str_arr[1] = u'\0';
    cOut->OutputString(cOut, str_arr);
}
