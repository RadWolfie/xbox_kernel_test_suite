#include <wchar.h>
#include <string.h>

#include "rtl.h"
#include "defines.h"

BOOL assert_ansi_string_ex(
    const ANSI_STRING* string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PCSTR expected_Buffer,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER(test_name);

    GEN_CHECK_EX(string->Length, expected_Length, "Length", line_number);
    GEN_CHECK_EX(string->MaximumLength, expected_MaximumLength, "MaximumLength", line_number);

    if (expected_Buffer == NULL) {
        GEN_CHECK_EX(string->Buffer, NULL, "BufferIsNull", line_number);
    }
    else {
        int result = memcmp(string->Buffer, expected_Buffer, expected_Length);
        GEN_CHECK_EX(result, 0, "BufferMemcmpResult", line_number);
        if (result) {
            print("  Expected Buffer = %.*s, Got = %.*s", expected_Length, expected_Buffer, string->Length, string->Buffer);
        }
    }

    ASSERT_FOOTER(test_name);
}

BOOL assert_unicode_string_ex(
    const UNICODE_STRING* string,
    USHORT expected_Length,
    USHORT expected_MaximumLength,
    PCWSTR expected_Buffer,
    const char* test_name,
    int line_number)
{
    ASSERT_HEADER(test_name);

    GEN_CHECK_EX(string->Length, expected_Length, "Length", line_number);
    GEN_CHECK_EX(string->MaximumLength, expected_MaximumLength, "MaximumLength", line_number);

    if (expected_Buffer == NULL) {
        GEN_CHECK_EX(string->Buffer, NULL, "BufferIsNull", line_number);
    }
    else {
        int result = memcmp(string->Buffer, expected_Buffer, expected_Length);
        GEN_CHECK_EX(result, 0, "BufferMemcmpResult", line_number);
        if (result) {
            print("  Expected Buffer = %.*ls, Got = %.*ls", expected_Length / 2, expected_Buffer, string->Length / 2, string->Buffer);
        }
    }

    ASSERT_FOOTER(test_name);
}
