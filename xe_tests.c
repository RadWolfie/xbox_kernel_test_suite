#include <xboxkrnl/xboxkrnl.h>
#include "output.h"
#include "assertion_defines.h"
#include "common_assertions.h"

void test_XeImageFileName(){
    /* FIXME: This is a stub! implement this function! */
}

void test_XeLoadSection(){
    /* FIXME: This is a stub! implement this function! */
#if 0
    const char* func_num = "0x0147";
    const char* func_name = "XeLoadSection";
    BOOL test_passed = 1;
    NTSTATUS result;

    print_test_header(func_num, func_name);

    result = XeLoadSection(NULL);
    GEN_CHECK(result, STATUS_INVALID_HANDLE, "thread invalid test");
    test_passed &= assert_NTSTATUS(result, STATUS_INVALID_HANDLE, func_name);

    print_test_footer(func_num, func_name, test_passed);
#endif
}

void test_XeUnloadSection(){
    /* FIXME: This is a stub! implement this function! */
}

void test_XePublicKeyData(){
    /* FIXME: This is a stub! implement this function! */
}
