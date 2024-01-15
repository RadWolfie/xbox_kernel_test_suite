#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

void test_ObDirectoryObjectType()
{
    const char* func_num = "0x00F0";
    const char* func_name = "ObDirectoryObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ObDirectoryObjectType, 'eriD', FALSE, FALSE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}

void test_ObSymbolicLinkObjectType()
{
    const char* func_num = "0x00F9";
    const char* func_name = "ObSymbolicLinkObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ObSymbolicLinkObjectType, 'bmyS', FALSE, TRUE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}
