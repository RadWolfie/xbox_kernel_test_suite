#include <hal/fileio.h>
#include <stdio.h>
#include <limits.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "assertions/common.h"

void test_NtCreateFile()
{
    const char* func_num = "0x00BE";
    const char* func_name = "NtCreateFile";
    BOOL test_passed = 1;
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    char buffer[200];

    print_test_header(func_num, func_name);

    XConvertDOSFilenameToXBOX("./default.xbe", filepath);
    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes = OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
        &handle,
        GENERIC_READ,
        &obj,
        &isb,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT);

    test_passed &= NT_SUCCESS(status);
    if (NT_SUCCESS(status)) {
        NtClose(handle);
    }

    // Series of file creation tests

    typedef struct {
        const unsigned char char_bad;
        NTSTATUS expected_result;
    } create_file_dir_test;

    // List of file/directory creation failures for specific character on hardware testing.
    create_file_dir_test file_dir_test_fail[] = {
        { 0, STATUS_OBJECT_NAME_COLLISION },
        { 1, STATUS_OBJECT_NAME_INVALID },
        { 2, STATUS_OBJECT_NAME_INVALID },
        { 3, STATUS_OBJECT_NAME_INVALID },
        { 4, STATUS_OBJECT_NAME_INVALID },
        { 5, STATUS_OBJECT_NAME_INVALID },
        { 6, STATUS_OBJECT_NAME_INVALID },
        { 7, STATUS_OBJECT_NAME_INVALID },
        { 8, STATUS_OBJECT_NAME_INVALID },
        { 9, STATUS_OBJECT_NAME_INVALID },
        { 10, STATUS_OBJECT_NAME_INVALID },
        { 11, STATUS_OBJECT_NAME_INVALID },
        { 12, STATUS_OBJECT_NAME_INVALID },
        { 13, STATUS_OBJECT_NAME_INVALID },
        { 14, STATUS_OBJECT_NAME_INVALID },
        { 15, STATUS_OBJECT_NAME_INVALID },
        { 16, STATUS_OBJECT_NAME_INVALID },
        { 17, STATUS_OBJECT_NAME_INVALID },
        { 18, STATUS_OBJECT_NAME_INVALID },
        { 19, STATUS_OBJECT_NAME_INVALID },
        { 20, STATUS_OBJECT_NAME_INVALID },
        { 21, STATUS_OBJECT_NAME_INVALID },
        { 22, STATUS_OBJECT_NAME_INVALID },
        { 23, STATUS_OBJECT_NAME_INVALID },
        { 24, STATUS_OBJECT_NAME_INVALID },
        { 25, STATUS_OBJECT_NAME_INVALID },
        { 26, STATUS_OBJECT_NAME_INVALID },
        { 27, STATUS_OBJECT_NAME_INVALID },
        { 28, STATUS_OBJECT_NAME_INVALID },
        { 29, STATUS_OBJECT_NAME_INVALID },
        { 30, STATUS_OBJECT_NAME_INVALID },
        { 31, STATUS_OBJECT_NAME_INVALID },
        { 34, STATUS_OBJECT_NAME_INVALID },
        { 42, STATUS_OBJECT_NAME_INVALID },
        { 43, STATUS_OBJECT_NAME_INVALID },
        { 44, STATUS_OBJECT_NAME_INVALID },
        { 46, STATUS_OBJECT_NAME_INVALID },
        { 47, STATUS_OBJECT_NAME_INVALID },
        { 58, STATUS_OBJECT_NAME_INVALID },
        { 59, STATUS_OBJECT_NAME_INVALID },
        { 60, STATUS_OBJECT_NAME_INVALID },
        { 61, STATUS_OBJECT_NAME_INVALID },
        { 62, STATUS_OBJECT_NAME_INVALID },
        { 63, STATUS_OBJECT_NAME_INVALID },
        { 92, STATUS_OBJECT_NAME_INVALID },
        { 124, STATUS_OBJECT_NAME_INVALID },
    };

    // Setup object attributes
    obj.RootDirectory = ObDosDevicesDirectory();
    obj.Attributes = 0; // We don't need OBJ_CASE_INSENSITIVE since we want to perform full test.
    obj.ObjectName = &obj_name;

    // We want to set initial value to max value in
    // order reset itself to zero on start of test.
    unsigned char char_test = UCHAR_MAX;
    do {
        char_test++;

        // Generate file path string to be tested.
        snprintf(filepath, sizeof(filepath), "D:\\%c", char_test);
        RtlInitAnsiString(&obj_name, filepath);

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

        // if we encounter a successful event, clean up our mess afterward.
        if (NT_SUCCESS(status)) {
            NtClose(handle);
            NtDeleteFile(&obj);
        }

        NTSTATUS expected_status = STATUS_SUCCESS;
        // Obtain expected failure if any from specific character from table.
        for (unsigned i = 0; i < (sizeof(file_dir_test_fail) / sizeof(file_dir_test_fail[0])); i++) {
            if (file_dir_test_fail[i].char_bad == char_test) {
                expected_status = file_dir_test_fail[i].expected_result;
                break;
            }
        }
        snprintf(buffer, sizeof(buffer), "file:char_test=%hhu;status", char_test);
        GEN_CHECK(status, expected_status, buffer);
    }
    while(char_test < UCHAR_MAX);

    // Series of directory creation tests

    // We want to set initial value to max value in
    // order reset itself to zero on start of test.
    char_test = UCHAR_MAX;
    do {
        char_test++;

        // Generate directory path string to be tested.
        snprintf(filepath, sizeof(filepath), "D:\\%c", char_test);
        RtlInitAnsiString(&obj_name, filepath);

        status = NtCreateFile(
            &handle,
            GENERIC_READ,
            &obj,
            &isb,
            NULL,
            FILE_ATTRIBUTE_NORMAL,
            0,
            FILE_CREATE,
            FILE_SYNCHRONOUS_IO_NONALERT);

        // if we encounter a successful event, clean up our mess afterward.
        if (NT_SUCCESS(status)) {
            NtClose(handle);
            NtDeleteFile(&obj);
        }

        NTSTATUS expected_status = STATUS_SUCCESS;
        // Obtain expected failure if any from specific character from table.
        for (unsigned i = 0; i < (sizeof(file_dir_test_fail) / sizeof(file_dir_test_fail[0])); i++) {
            if (file_dir_test_fail[i].char_bad == char_test) {
                expected_status = file_dir_test_fail[i].expected_result;
                break;
            }
        }
        snprintf(buffer, sizeof(buffer), "dir:char_test=%hhu;status", char_test);
        GEN_CHECK(status, expected_status, buffer);
    }
    while(char_test < UCHAR_MAX);

    print_test_footer(func_num, func_name, test_passed);
}
