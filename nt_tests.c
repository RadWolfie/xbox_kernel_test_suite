#include <hal/fileio.h>
#include <stdio.h>
#include <limits.h>
#include "output.h"
#include "assertion_defines.h"
#include "common_assertions.h"

void test_NtAllocateVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCancelTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtClearEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtClose(){
    const char* func_num = "0x00BB";
    const char* func_name = "NtClose";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    BOOL tests_passed = 0;

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

    if(NtClose(handle)==0)
        tests_passed += 1;

    print_test_footer(func_num, func_name, tests_passed);
}

void test_NtCreateDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateFile(){
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

void test_NtCreateIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtCreateTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtDeleteFile(){
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

void test_NtDeviceIoControlFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtDuplicateObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFlushBuffersFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFreeVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtFsControlFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtOpenSymbolicLinkObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtProtectVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtPulseEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryDirectoryFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueueApcThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryDirectoryObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryFullAttributesFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQuerySemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQuerySymbolicLinkObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryVirtualMemory(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtQueryVolumeInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReadFile(){ // We will read the xbe magic number as a test
    const char* func_num = "0x00DB";
    const char* func_name = "NtReadFile";
    HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];
    ULONG uSize = 0x0004;
    char read[0x0004];
    BOOL tests_passed = 1;

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

    if (!NT_SUCCESS(status)) {
        NtClose(handle);
        tests_passed = 0;
        print_test_footer(func_num, func_name, tests_passed);
        return;
    }

    status = NtReadFile(
    handle,
    NULL,
    NULL,
    NULL,
    &isb,
    read,
    uSize,
    NULL);

    if (status == STATUS_PENDING)
        status = NtWaitForSingleObject((void*)handle, FALSE, (void*)NULL);

    tests_passed &= NT_SUCCESS(status);
    NtClose(handle);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_NtReadFileScatter(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReleaseMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtReleaseSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtRemoveIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetInformationFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetIoCompletion(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetSystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSetTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSignalAndWaitForSingleObjectEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtSuspendThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtUserIoApcDispatcher(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForSingleObject(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForSingleObjectEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWaitForMultipleObjectsEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWriteFile(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtWriteFileGather(){
    /* FIXME: This is a stub! implement this function! */
}

void test_NtYieldExecution(){
    /* FIXME: This is a stub! implement this function! */
}
