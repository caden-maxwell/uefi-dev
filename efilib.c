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

void PutChar(CHAR16 ch) {
    // ConOut->OutputString expects a null-terminated string of chars
    CHAR16 str_arr[2] = { ch, u'\0' };
    cOut->OutputString(cOut, str_arr);
}

void PrintInt(INT32 num)
{
    CHAR16 buffer[12]; // 1 sign char, 10 num chars, 1 null-terminator

    BOOLEAN negative = num < 0;
    if (negative) num = -num;

    UINTN i = 0;
    do {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (negative) buffer[i++] = u'-';
    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--) {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    cOut->OutputString(cOut, buffer);
}

void PrintUint(UINT32 num)
{
    CHAR16 buffer[11]; // 10 num chars, 1 null-terminator

    UINTN i = 0;
    do {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--) {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    cOut->OutputString(cOut, buffer);
}

void PrintHex(UINT32 num) // Expects an unsigned int
{
    CHAR16 *digits = u"0123456789ABCDEF";
    CHAR16 buffer[9]; // 8 hex chars, 1 null-terminator

    UINTN i = 0;
    do {
        buffer[i++] = digits[num % 16];
        num /= 16;
    } while (num > 0);

    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--) {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    cOut->OutputString(cOut, buffer);
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
                    PutChar(u'%');
                    break;
                }
                case u'd': // '%d' -- signed int
                {
                    PrintInt(va_arg(args, INT32));
                    break;
                }
                case u'x': // '%x' -- hexadecimal
                {
                    PrintHex(va_arg(args, UINT32));
                    break;
                }
                case u'u': // '%u' -- unsigned int
                {
                    PrintUint(va_arg(args, UINT32));
                    break;
                }
                case u'h':
                {
                    va_arg(args, UINTN); // Still need to consume an arg so we can continue
                    Printf(u"(Format specifier '%%%c' is not yet implemented)", ch);
                    break;
                }
                default:
                {
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
