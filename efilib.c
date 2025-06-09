#include "efilib.h"
#include <stdarg.h>

EFI_SYSTEM_TABLE *ST;
EFI_BOOT_SERVICES *BS;
EFI_RUNTIME_SERVICES *RS;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cIn;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cOut;
EFI_STATUS Status;

VOID InitGlobalVars(IN EFI_SYSTEM_TABLE *SystemTable)
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

VOID PutChar(IN CHAR16 ch)
{
    // ConOut->OutputString expects a null-terminated string of chars
    CHAR16 str_arr[2] = {ch, u'\0'};
    cOut->OutputString(cOut, str_arr);
}

// Expand upon PrintXXX in the future to use any type of int (byte, short, int, long)
// Buffer size would be ceil(log_(base)^(2^num_bits - 1)) + 1
VOID IntToStr(OUT CHAR16 *buffer, IN INT32 num)
{
    BOOLEAN negative = num < 0;
    if (negative)
        num = -num;

    UINTN i = 0;
    do
    {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (negative)
        buffer[i++] = u'-';
    buffer[i--] = u'\0';

    for (UINTN j = 0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

VOID UIntToStr(CHAR16 *buffer, UINT32 num)
{
    UINTN i = 0;
    do
    {
        buffer[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    buffer[i--] = u'\0';

    for (UINTN j = 0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

VOID HexToStr(CHAR16 *buffer, UINT32 hexnum) // Expects an unsigned int
{
    CHAR16 *digits = u"0123456789ABCDEF";
    UINTN i = 0;
    do
    {
        buffer[i++] = digits[hexnum % 16];
        hexnum /= 16;
    } while (hexnum > 0);

    buffer[i--] = u'\0';

    for (UINTN j = 0; j < i; j++, i--)
    {
        CHAR16 temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

VOID FormatString(OUT CHAR16 *str_buf, IN CHAR16 *fstr, IN va_list args)
{
    CHAR16 ch;
    UINTN buf_idx = 0;
    for (UINTN i = 0; fstr[i] != u'\0'; i++)
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
                while (*str)
                    str_buf[buf_idx++] = *str++;
                break;
            }
            case u'c': // '%c' -- character
            {
                CHAR16 chr = (CHAR16)va_arg(args, UINTN);
                if (chr)
                    str_buf[buf_idx++] = chr;
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
                while (*int_buf_ptr)
                    str_buf[buf_idx++] = *int_buf_ptr++;
                break;
            }
            case u'x': // '%x' -- hexadecimal
            {
                CHAR16 str_hex_buf[9]; // 8 hex chars, 1 null-terminator
                CHAR16 *hex_buf_ptr = str_hex_buf;
                HexToStr(str_hex_buf, va_arg(args, UINT32));
                while (*hex_buf_ptr)
                    str_buf[buf_idx++] = *hex_buf_ptr++;
                break;
            }
            case u'u': // '%u' -- unsigned int
            {
                CHAR16 str_uint_buf[11]; // 10 num chars, 1 null-terminator
                CHAR16 *uint_buf_ptr = str_uint_buf;
                UIntToStr(str_uint_buf, va_arg(args, UINT32));
                while (*uint_buf_ptr)
                    str_buf[buf_idx++] = *uint_buf_ptr++;
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

VOID vPrintf(IN CHAR16 *fstr, IN va_list args)
{
    CHAR16 buffer[1024];
    FormatString(buffer, fstr, args);
    cOut->OutputString(cOut, buffer);
}

VOID Printf(IN CHAR16 *fstr, ...)
{
    va_list args;
    va_start(args, fstr);
    vPrintf(fstr, args);
}

UINTN StrlCpy(OUT CHAR16 *dest, IN const CHAR16 *source, IN UINTN size)
{
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

VOID vsnPrintf(OUT CHAR16 *dest, IN UINTN destsize, IN CHAR16 *fstr, IN va_list args)
{
    CHAR16 tempbuf[1024];
    FormatString(tempbuf, fstr, args);
    StrlCpy(dest, tempbuf, destsize);
}

VOID snPrintf(OUT CHAR16 *dest, IN UINTN destsize, IN CHAR16 *fstr, ...)
{
    va_list args;
    va_start(args, fstr);
    vsnPrintf(dest, destsize, fstr, args);
}

// This is just to make the linker happy
VOID *memcpy(VOID *dest, const VOID *src, UINTN n)
{
    BS->CopyMem(dest, (VOID *)src, n);
    return dest;
}

BOOLEAN StrToUInt(IN CHAR16 *str, OUT UINTN *out)
{
    CHAR16 *s = str;
    const CHAR16 *cs = str;
    INT32 numlen = 0;
    while (*s != '\0' && *s >= '0' && *s++ <= '9')
        numlen++;

    if (numlen == 0)
        return FALSE;

    UINTN sum = 0;
    for (INT32 i = numlen - 1, j = 0; i >= 0; i--, j++)
    {
        UINT32 tmp = cs[i] - '0';
        sum += tmp * pow(10, j);
    }
    *out = sum;
    return TRUE;
}

UINTN pow(IN UINTN base, IN UINTN exponent)
{
    UINTN num = 1;
    for (UINT32 i = 0; i < exponent; i++)
        num *= base;
    return num;
}
