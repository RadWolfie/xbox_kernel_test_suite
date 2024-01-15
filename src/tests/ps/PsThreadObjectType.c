#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/object_type.h"

void test_PsThreadObjectType()
{
    const char* func_num = "0x0103";
    const char* func_name = "PsThreadObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&PsThreadObjectType, 'erhT', FALSE, FALSE, FALSE);

    print_test_footer(func_num, func_name, test_passed);
}
