#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

void test_IoCompletionObjectType()
{
    const char* func_num = "0x0040";
    const char* func_name = "IoCompletionObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&IoCompletionObjectType, 'pmoC', FALSE, TRUE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_IoDeviceObjectType()
{
    const char* func_num = "0x0046";
    const char* func_name = "IoDeviceObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&IoDeviceObjectType, 'iveD', FALSE, FALSE, TRUE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_IoFileObjectType()
{
    const char* func_num = "0x0047";
    const char* func_name = "IoFileObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&IoFileObjectType, 'eliF', TRUE, TRUE, TRUE);

    print_test_footer(func_num, func_name, test_passed);
}
