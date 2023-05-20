#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "assertions/common.h"

void test_NtQuerySymbolicLinkObject()
{
    const char* func_num = "0x00D7";
    const char* func_name = "NtQuerySymbolicLinkObject";
    BOOL test_passed = 1;
    NTSTATUS result;
    HANDLE handle;
    OBJECT_ATTRIBUTES objattr;
    const char* drive_buffer = "\\??\\D:";
    ANSI_STRING drive_path;
    STRING link_target = { 0, 0, NULL };
    char* str_buffer;
    ULONG length_return;

    print_test_header(func_num, func_name);

    // temporary open D drive letter
    RtlInitAnsiString(&drive_path, drive_buffer);
    InitializeObjectAttributes(&objattr, &drive_path, OBJ_CASE_INSENSITIVE, NULL, NULL);
    result = NtOpenSymbolicLinkObject(&handle, &objattr);
    GEN_CHECK(result, STATUS_SUCCESS, "D drive result");

    if (result == STATUS_SUCCESS) {
        result = NtQuerySymbolicLinkObject(handle, &link_target, &length_return);
        test_passed &= assert_NTSTATUS(result, STATUS_BUFFER_TOO_SMALL, func_name);

        str_buffer = (char*)malloc(length_return);
        if (str_buffer) {
            link_target.Buffer = str_buffer;
            link_target.MaximumLength = length_return;

            result = NtQuerySymbolicLinkObject(handle, &link_target, &length_return);
            test_passed &= assert_NTSTATUS(result, STATUS_SUCCESS, func_name);

            link_target.Buffer = NULL;
            link_target.MaximumLength = 0;
            free(str_buffer);
        }

        NtClose(handle);
    }

    result = NtQuerySymbolicLinkObject(NULL, &link_target, &length_return);
    GEN_CHECK(result, STATUS_INVALID_HANDLE, "null input result");
    test_passed &= assert_NTSTATUS(result, STATUS_INVALID_HANDLE, func_name);

    print_test_footer(func_num, func_name, test_passed);
}
