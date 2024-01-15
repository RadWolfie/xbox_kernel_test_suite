#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

void test_ExEventObjectType()
{
    const char* func_num = "0x0010";
    const char* func_name = "ExEventObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ExEventObjectType, 'vevE', FALSE, FALSE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_ExMutantObjectType()
{
    const char* func_num = "0x0016";
    const char* func_name = "ExMutantObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ExMutantObjectType, 'atuM', FALSE, TRUE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_ExSemaphoreObjectType()
{
    const char* func_num = "0x001E";
    const char* func_name = "ExSemaphoreObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ExSemaphoreObjectType, 'ameS', FALSE, FALSE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_ExTimerObjectType()
{
    const char* func_num = "0x001F";
    const char* func_name = "ExTimerObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ExTimerObjectType, 'emiT', FALSE, TRUE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}
