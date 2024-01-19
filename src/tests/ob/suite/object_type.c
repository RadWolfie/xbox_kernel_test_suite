#include <xboxkrnl/xboxkrnl.h>
#include <nxdk/mount.h>
#include <handleapi.h>

#include "util/output.h"
#include "assertions/object_type.h"

void test_ObDirectoryObjectType()
{
    const char* func_num = "0x00F0";
    const char* func_name = "ObDirectoryObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ObDirectoryObjectType, 'eriD', FALSE, FALSE, FALSE);
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, NULL, OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);
    HANDLE directory_handle;
    NTSTATUS status = NtOpenDirectoryObject(&directory_handle, &obj_attr);
    if (NT_SUCCESS(status)) {
        test_passed &= assert_object_header_type(&ObDirectoryObjectType, directory_handle);
        BOOL close = CloseHandle(directory_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }

    print_test_footer(func_num, func_name, test_passed);
}

void test_ObSymbolicLinkObjectType()
{
    const char* func_num = "0x00F9";
    const char* func_name = "ObSymbolicLinkObjectType";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    test_passed &= assert_object_type(&ObSymbolicLinkObjectType, 'bmyS', FALSE, TRUE, FALSE);
    ANSI_STRING obj_name;
    RtlInitAnsiString(&obj_name, "\\??\\A:");
    bool is_mount = nxMountDrive(obj_name.Buffer[4], "\\Device\\CdRom0");
    GEN_CHECK(is_mount, TRUE, "is_mount");
    OBJECT_ATTRIBUTES obj_attr;
    InitializeObjectAttributes(&obj_attr, &obj_name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    HANDLE symlink_handle;
    NTSTATUS status = NtOpenSymbolicLinkObject(&symlink_handle, &obj_attr);
    if (NT_SUCCESS(status)) {
        test_passed &= assert_object_header_type(&ObSymbolicLinkObjectType, symlink_handle);
        BOOL close = CloseHandle(symlink_handle);
        GEN_CHECK(close, TRUE, "close handle");
    }
    if (is_mount) {
        nxUnmountDrive(obj_name.Buffer[4]);
    }

    print_test_footer(func_num, func_name, test_passed);
}
