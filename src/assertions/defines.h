#pragma once

#include "util/output.h"

#define TEST_VERBOSE get_verbose_value()

#define ASSERT_HEADER(test_name) \
    BOOL test_passed = 1; \
    if (TEST_VERBOSE) { \
        print("  Test '%s' Starting", test_name); \
    }

#define ASSERT_FOOTER(test_name) \
    if (!test_passed) { \
        print("  Test '%s' FAILED", test_name); \
    } \
    else if (TEST_VERBOSE) { \
        print("  Test '%s' PASSED", test_name); \
    } \
    return test_passed

#define GEN_CHECK_EX(check_var, expected_var, varname, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected %s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected %s = 0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): %s = 0x%llX" : \
                "  OK(line %d): %s = 0x%X" \
            ), \
            func_line, varname, (check_var) \
        ); \
    }
#define GEN_CHECK(check_var, expected_var, varname) GEN_CHECK_EX(check_var, expected_var, varname, __LINE__)

#define GEN_CHECK_RANGE_EX(check_var, expected_var, length, varname, func_line) \
    if ((check_var) < (expected_var) || (check_var) > (expected_var) + (length)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected range %s = 0x%llX-0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected range %s = 0x%X-0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (expected_var), (expected_var) + (length), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): range %s = 0x%llX" : \
                "  OK(line %d): range %s = 0x%X" \
            ), \
            func_line, varname, (check_var) \
        ); \
    }
#define GEN_CHECK_RANGE(check_var, expected_var, length, varname) GEN_CHECK_RANGE_EX(check_var, expected_var, length, varname, __LINE__)

#define GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        if ((check_var)[i] != (expected_var)[i]) { \
            print( \
                ((sizeof((check_var)[i]) > 4) ? \
                    "  ERROR(line %d): Expected array %s[%u] = 0x%llX, Got = 0x%llX" : \
                    "  ERROR(line %d): Expected array %s[%u] = 0x%X, Got = 0x%X" \
                ), \
                func_line, varname, i, (expected_var)[i], (check_var)[i] \
            ); \
            TEST_FAILED(); \
        } \
        else if (TEST_VERBOSE) { \
            print( \
                ((sizeof((check_var)[i]) > 4) ? \
                    "  OK(line %d): array %s[%u] = 0x%llX" : \
                    "  OK(line %d): array %s[%u] = 0x%X" \
                ), \
                func_line, varname, i, (check_var)[i] \
            ); \
        } \
    }
#define GEN_CHECK_ARRAY(check_var, expected_var, size, varname) GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        if ((var)[i].m_check != (var)[i].m_expected) { \
            print( \
                ((sizeof((var)[i].m_check) > 4) ? \
                    "  ERROR(line %d): Expected array member %s[%u].%s = 0x%llX, Got %s[%u].%s = 0x%llX" : \
                    "  ERROR(line %d): Expected array member %s[%u].%s = 0x%X, Got %s[%u].%s = 0x%X" \
                ), \
                func_line, varname, i, #m_expected, (var)[i].m_expected, varname, i, #m_check, (var)[i].m_check \
            ); \
            TEST_FAILED(); \
        } \
        else if (TEST_VERBOSE) { \
            print( \
                ((sizeof((var)[i].m_check) > 4) ? \
                    "  OK(line %d): array member %s[%u].%s = 0x%llX" : \
                    "  OK(line %d): array member %s[%u].%s = 0x%X" \
                ), \
                func_line, varname, i, #m_expected, (var)[i].m_check \
            ); \
        } \
    }
#define GEN_CHECK_ARRAY_MEMBER(var, m_check, m_expected, size, varname) GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, __LINE__)
