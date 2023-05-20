#include <xboxkrnl/xboxkrnl.h>
#include <stdio.h>

#include "util/output.h"

void test_NtDeleteFile()
{
    const char* func_num = "0x00C3";
    const char* func_name = "NtDeleteFile";
    BOOL test_passed = 1;
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];

    print_test_header(func_num, func_name);

    snprintf(filepath, sizeof(filepath), "D:\\%s", func_name);
    RtlInitAnsiString(&obj_name, filepath);

    // Setup object attributes
    obj.RootDirectory = ObDosDevicesDirectory();
    obj.Attributes = 0; // We don't need OBJ_CASE_INSENSITIVE since we want to perform full test.
    obj.ObjectName = &obj_name;

    // First, let's try delete nonexistent file.
    status = NtDeleteFile(&obj);
    test_passed &= assert_NTSTATUS(status, STATUS_OBJECT_NAME_NOT_FOUND, func_name);

    // Then create the file and delete it afterward.
    status = NtCreateFile(
        &handle,
        GENERIC_READ,
        &obj,
        &isb,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_CREATE,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE);

    if (NT_SUCCESS(status)) {
        NtClose(handle);
    }

    status = NtDeleteFile(&obj);

    test_passed &= assert_NTSTATUS(status, STATUS_SUCCESS, func_name);

    print_test_footer(func_num, func_name, test_passed);
}
