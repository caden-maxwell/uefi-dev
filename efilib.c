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
BOOLEAN Printf(CHAR16 *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    CHAR16 ch;
    CHAR16 str_arr[2]; // Implement memset so we can shorthand this
    str_arr[0] = u'\0';
    str_arr[1] = u'\0';
    CHAR16 *str = str_arr;
    for (UINTN i=0; fmt[i] != u'\0'; i++) {
        ch = fmt[i];
        // If we see a format specifier '%'...
        if (ch == u'%') {
            i++;
            // Check the kind of format specifier
            // If it's a string:
            ch = fmt[i];
            if (ch == u's')
            {
                // Get the next arg as a string, and print the string
                CHAR16 *arg = va_arg(args, CHAR16 *);
                cOut->OutputString(cOut, arg);
            }
            // If it's a char:
            else if (ch == u'c')
            {
                // Get the next arg as an int, and print the int 
                CHAR16 arg = (CHAR16)va_arg(args, UINTN);
                CHAR16 ch_arr[2];
                ch_arr[0] = arg;
                ch_arr[1] = u'\0';
                CHAR16 *str2 = ch_arr;
                cOut->OutputString(cOut, str2);
            }
            // If it is an int:
            else if (ch == u'd')
            {
                // Get the next arg as an int, and print the int 
                UINTN arg = va_arg(args, UINTN);
                (void)arg;
            }
            else
                cOut->OutputString(cOut, u"(INVALID FORMAT SPECIFIER)");
        }
        // Otherwise, print the char
        else
        {
            str[0] = fmt[i];
            cOut->OutputString(cOut, str);
        }
    }

    va_end(args);
    return TRUE;
}

void PrintValues(int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        CHAR16 *str = va_arg(args, CHAR16 *);
        cOut->OutputString(cOut, str);
    }
    va_end(args);
}
