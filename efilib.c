#include "efilib.h"
#include <stdarg.h>

EFI_SYSTEM_TABLE  *ST;
EFI_BOOT_SERVICES *BS;
EFI_RUNTIME_SERVICES *RS;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
EFI_STATUS Status;

void InitGlobalVars(EFI_SYSTEM_TABLE *SystemTable)
{
    ST = SystemTable;
    BS = ST->BootServices;
    RS = ST->RuntimeServices;
    cIn = ST->ConIn;
    cOut = ST->ConOut;
}

// ===========================
// ===== Print functions =====
// ===========================

void PutChar(CHAR16 ch)
{
    // ConOut->OutputString expects a null-terminated string of chars
    CHAR16 str_arr[2] = { ch, u'\0' };
    cOut->OutputString(cOut, str_arr);
}

// Expand upon PrintXXX in the future to use any type of int (byte, short, int, long)
// Buffer size would be ceil(log_(base)^(2^num_bits - 1)) + 1
void IntToStr(CHAR16 *buffer, INT32 num)
{
    BOOLEAN negative = num < 0;
    if (negative) num = -num;

    UINTN i = 0;
    do {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (negative) buffer[i++] = u'-';
    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

void UIntToStr(CHAR16 *buffer, UINT32 num)
{
    UINTN i = 0;
    do {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

void HexToStr(CHAR16 *buffer, UINT32 hexnum) // Expects an unsigned int
{
    CHAR16 *digits = u"0123456789ABCDEF";
    UINTN i = 0;
    do {
        buffer[i++] = digits[hexnum % 16];
        hexnum /= 16;
    } while (hexnum > 0);

    buffer[i--] = u'\0';

    for (UINTN j=0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

VOID FormatString(OUT CHAR16 *str_buf, CHAR16 *fstr, va_list args)
{
    CHAR16 ch;
    UINTN buf_idx = 0;
    for (UINTN i=0; fstr[i] != u'\0'; i++)
    {
        ch = fstr[i];
        // If we see a format specifier '%'...
        if (ch == u'%')
        {
            ch = fstr[++i];
            // Check the kind of format specifier
            switch (ch)
            {
                case u's': // '%s' -- string
                {
                    CHAR16 *str = va_arg(args, CHAR16 *);
                    while (*str) str_buf[buf_idx++] = *str++;
                    break;
                }
                case u'c': // '%c' -- character
                {
                    CHAR16 chr = (CHAR16)va_arg(args, UINTN);
                    if (chr) str_buf[buf_idx++] = chr;
                    break;
                }
                case u'%': // '%%' -- literal %
                {
                    str_buf[buf_idx++] = u'%';
                    break;
                }
                case u'd': // '%d' -- signed int
                {
                    CHAR16 str_int_buf[12]; // 1 sign char, 10 num chars, 1 null-terminator
                    CHAR16 *int_buf_ptr = str_int_buf;
                    IntToStr(str_int_buf, va_arg(args, INT32));
                    while (*int_buf_ptr) str_buf[buf_idx++] = *int_buf_ptr++;
                    break;
                }
                case u'x': // '%x' -- hexadecimal
                {
                    CHAR16 str_hex_buf[9]; // 8 hex chars, 1 null-terminator
                    CHAR16 *hex_buf_ptr = str_hex_buf;
                    HexToStr(str_hex_buf, va_arg(args, UINT32));
                    while (*hex_buf_ptr) str_buf[buf_idx++] = *hex_buf_ptr++;
                    break;
                }
                case u'u': // '%u' -- unsigned int
                {
                    CHAR16 str_uint_buf[11]; // 10 num chars, 1 null-terminator
                    CHAR16 *uint_buf_ptr = str_uint_buf;
                    UIntToStr(str_uint_buf, va_arg(args, UINT32));
                    while (*uint_buf_ptr) str_buf[buf_idx++] = *uint_buf_ptr++;
                    break;
                }
                case u'h':
                {
                    // TODO: Fix messy recursion here
                    va_arg(args, UINTN); // Still need to consume an arg so we can continue
                    Printf(u"(Format specifier '%%%c' is not yet implemented)", ch);
                    break;
                }
                default:
                {
                    // TODO: Fix messy recursion here
                    va_arg(args, UINTN);
                    Printf(u"(INVALID FORMAT SPECIFIER: '%%%c')", ch);
                }
            }
        }
        // Otherwise, print the char
        else
            str_buf[buf_idx++] = ch;
    }

    str_buf[buf_idx] = u'\0';
    va_end(args);
}

VOID vPrintf(CHAR16 *fstr, va_list args)
{
    CHAR16 buffer[1024];
    FormatString(buffer, fstr, args);
    cOut->OutputString(cOut, buffer);
}

VOID Printf(CHAR16 *fstr, ...)
{
    va_list args;
    va_start(args, fstr);
    vPrintf(fstr, args);
}

VOID vsPrintf(CHAR16 *buf, CHAR16 *fstr, va_list args)
{
    FormatString(buf, fstr, args);
}

VOID sPrintf(OUT CHAR16 *buf, IN CHAR16 *fstr, ...)
{
    va_list args;
    va_start(args, fstr);
    vsPrintf(buf, fstr, args);
}

UINTN strlcpy(OUT CHAR16 *dest, const CHAR16 *source, UINTN size) {
    CHAR16 *dst = dest;
    const CHAR16 *src = source;
    CHAR16 *end = dest + size;
    while (*src != '\0' && dst < end)
        *dst++ = *src++;

    if (dst < end)
        *dst = 0;
    else if (size > 0)
        dst[-1] = 0;

    while (*src != '\0')
        src++;

    return src - source;
}

VOID vsnPrintf(OUT CHAR16 *dest, INT32 n, CHAR16 *fstr, va_list args)
{
    CHAR16 tempbuf[1024];
    FormatString(tempbuf, fstr, args);
    strlcpy(dest, tempbuf, n);
}

VOID snPrintf(OUT CHAR16 *buf, IN INT32 n, IN CHAR16 *fstr, ...)
{
    va_list args;
    va_start(args, fstr);
    vsnPrintf(buf, n, fstr, args);
}

VOID *memcpy(VOID *dest, const VOID *src, UINTN n) {
    BS->CopyMem(dest, (VOID *)src, n);
    return dest;
}
