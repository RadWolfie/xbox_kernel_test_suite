#pragma once

#include <xboxkrnl/xboxkrnl.h>

#include "defines.h"

#define assert_NTSTATUS_ex( \
    status, \
    expected_status, \
    api_name, \
    line_number \
) \
    if (status != expected_status) { \
        print( \
            "  ERROR(line %d): Expected return status of function '%s' = 0x%X, got = 0x%X", \
            line_number, \
            api_name, \
            expected_status, \
            status \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            "  OK(line %d): return status of function '%s' = 0x%X", \
            line_number, \
            api_name, \
            status \
        ); \
    }
#define assert_NTSTATUS( \
    status, \
    expected_status, \
    api_name \
) \
assert_NTSTATUS_ex( \
    status, \
    expected_status, \
    api_name, \
    __LINE__ \
)
