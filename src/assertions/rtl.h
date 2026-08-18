#pragma once

#include <xboxkrnl/xboxkrnl.h>

#include "defines.h"

#define assert_critical_section_equals_ex( \
    crit_section, \
    expected_LockCount, \
    expected_RecursionCount, \
    expected_OwningThread, \
    line_number \
) \
    GEN_CHECK_EX((crit_section)->LockCount, expected_LockCount, ".LockCount", line_number); \
    GEN_CHECK_EX((crit_section)->RecursionCount, expected_RecursionCount, ".RecursionCount", line_number); \
    GEN_CHECK_EX((crit_section)->OwningThread, expected_OwningThread, ".OwningThread", line_number)
#define assert_critical_section_equals( \
    crit_section, \
    expected_LockCount, \
    expected_RecursionCount, \
    expected_OwningThread \
) \
assert_critical_section_equals_ex( \
    crit_section, \
    expected_LockCount, \
    expected_RecursionCount, \
    expected_OwningThread, \
    __LINE__ \
)

BOOL assert_ansi_string_ex(
    const ANSI_STRING*,
    USHORT,
    USHORT,
    PCSTR,
    const char*,
    int
);
#define assert_ansi_string( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name \
) \
test_passed &= assert_ansi_string_ex( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name, \
    __LINE__ \
)

BOOL assert_unicode_string_ex(
    const UNICODE_STRING*,
    USHORT,
    USHORT,
    PCWSTR,
    const char*,
    int
);
#define assert_unicode_string( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name \
) \
test_passed &= assert_unicode_string_ex( \
    string, \
    expected_Length, \
    expected_MaximumLength, \
    expected_Buffer, \
    test_name, \
    __LINE__ \
)
