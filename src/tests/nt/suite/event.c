#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(NtClearEvent)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(NtCreateEvent)
{
    TEST_BEGIN();

    HANDLE handle, handle2;
    PVOID object;
    NTSTATUS status;
    ANSI_STRING obj_name;

    RtlInitAnsiString(&obj_name, TEST_GET_API_NAME);
    OBJECT_ATTRIBUTES obj_attr;
    obj_attr.ObjectName = &obj_name;

    // Test #1, validate RootDirectory inputs
    typedef struct {
        HANDLE input;
        NTSTATUS expected_status;
        NTSTATUS return_status;
    } RootDirectory_test;
    RootDirectory_test RootDirectory_tests[] = {
        { .input = NULL, .expected_status = STATUS_OBJECT_NAME_INVALID },
        { .input = ObDosDevicesDirectory(), .expected_status = 0 }, // TODO: Why did this become successful?
        { .input = ObWin32NamedObjectsDirectory(), .expected_status = 0 },
    };
    obj_attr.Attributes = 0;
    for (unsigned i = 0; i < ARRAY_SIZE(RootDirectory_tests); i++) {
        RootDirectory_test* test = &RootDirectory_tests[i];
        obj_attr.RootDirectory = test->input;
        test->return_status = NtCreateEvent(&handle,
                                            &obj_attr,
                                            NotificationEvent,
                                            FALSE);
        if (NT_SUCCESS(RootDirectory_tests[i].return_status)) {
            (void)NtClose(handle);
        }
    }
    GEN_CHECK_ARRAY_MEMBER(RootDirectory_tests, return_status, expected_status, ARRAY_SIZE(RootDirectory_tests), "RootDirectory_tests");

    // Test #2, validate Attributes inputs
    obj_attr.RootDirectory = ObWin32NamedObjectsDirectory(); // default

    typedef struct {
        ULONG input;
        BOOL expected_handle;
        BOOL expected_handle2;
        NTSTATUS expected_status;
        NTSTATUS expected_status2;
        BOOL return_handle;
        BOOL return_handle2;
        NTSTATUS return_status;
        NTSTATUS return_status2;
    } Attributes_test;
    Attributes_test Attributes_tests[] = {
        { .input = 0, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = OBJ_INHERIT, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = OBJ_PERMANENT, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = OBJ_EXCLUSIVE, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = OBJ_CASE_INSENSITIVE, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = OBJ_OPENIF, .expected_handle = TRUE, .expected_handle2 = TRUE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_EXISTS },
        { .input = OBJ_OPENLINK, .expected_handle = TRUE, .expected_handle2 = FALSE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_COLLISION },
        { .input = 0xFFFFFFFF, .expected_handle = TRUE, .expected_handle2 = TRUE, .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_OBJECT_NAME_EXISTS },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(Attributes_tests); i++) {
        handle = handle2 = NULL;
        Attributes_test* test = &Attributes_tests[i];
        obj_attr.Attributes = test->input;
        test->return_status = NtCreateEvent(&handle,
                                            &obj_attr,
                                            NotificationEvent,
                            FALSE);
        test->return_handle = (handle != NULL);
        test->return_status2 = NtCreateEvent(&handle2,
                                             &obj_attr,
                                             NotificationEvent,
                                             FALSE);
        test->return_handle2 = (handle2 != NULL);
        if (NT_SUCCESS(test->return_status2)) {
            (void)NtClose(handle2);
        }
        if (NT_SUCCESS(test->return_status)) {
            if (test->input & OBJ_PERMANENT) {
                status = ObReferenceObjectByHandle(handle, &ExEventObjectType, &object);
                if (NT_SUCCESS(status)) {
                    ObMakeTemporaryObject(object);
                    ObfDereferenceObject(object);
                }
            }
            (void)NtClose(handle);
        }
    }
    GEN_CHECK_ARRAY_MEMBER(Attributes_tests, return_handle, expected_handle, ARRAY_SIZE(Attributes_tests), "Attributes_tests");
    GEN_CHECK_ARRAY_MEMBER(Attributes_tests, return_handle2, expected_handle2, ARRAY_SIZE(Attributes_tests), "Attributes_tests");
    GEN_CHECK_ARRAY_MEMBER(Attributes_tests, return_status, expected_status, ARRAY_SIZE(Attributes_tests), "Attributes_tests");
    GEN_CHECK_ARRAY_MEMBER(Attributes_tests, return_status2, expected_status2, ARRAY_SIZE(Attributes_tests), "Attributes_tests");

    // Test #3, validate event types support
    obj_attr.Attributes = 0; // default

    typedef struct {
        EVENT_TYPE input_event_type; // NotificationEvent or SynchronizationEvent
        NTSTATUS expected_status;
        NTSTATUS return_status;
    } create_test;
    create_test create_tests[3] = {
        // NotificationEvent
        { .input_event_type = NotificationEvent, .expected_status = STATUS_SUCCESS },
        // SynchronizationEvent
        { .input_event_type = SynchronizationEvent, .expected_status = STATUS_SUCCESS },
        // Invalid
        { .input_event_type = 3, .expected_status = STATUS_INVALID_PARAMETER },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(create_tests); i++) {
        create_test* test = &create_tests[i];
        test->return_status = NtCreateEvent(&handle,
                                            &obj_attr,
                                            test->input_event_type,
                                            FALSE);
        if (NT_SUCCESS(test->return_status)) {
            (void)NtClose(handle);
        }
    }
    GEN_CHECK_ARRAY_MEMBER(create_tests, return_status, expected_status, ARRAY_SIZE(create_tests), "create_tests");

    TEST_END();
}

TEST_FUNC(NtPulseEvent)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(NtQueryEvent)
{
    TEST_BEGIN();

    HANDLE handle;
    PVOID object;
    NTSTATUS status;
    ANSI_STRING obj_name;

    // Test #1, test invalid handles
    EVENT_BASIC_INFORMATION info = { 0 };
    status = NtQueryEvent(NULL, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");
    GEN_CHECK(info.EventState, 0, "info.EventState");
    GEN_CHECK(info.EventType, 0, "info.EventType");
    status = NtQueryEvent((HANDLE)0xBEEF, &info);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");
    GEN_CHECK(info.EventState, 0, "info.EventState");
    GEN_CHECK(info.EventType, 0, "info.EventType");

    RtlInitAnsiString(&obj_name, TEST_GET_API_NAME);
    OBJECT_ATTRIBUTES obj_attr;
    obj_attr.ObjectName = &obj_name;
    InitializeObjectAttributes(&obj_attr, &obj_name, 0, ObWin32NamedObjectsDirectory(), NULL);

    typedef struct {
        NTSTATUS expected_status, expected_status2, expected_status3;
        EVENT_BASIC_INFORMATION expected_info, expected_info2, expected_info3;
        NTSTATUS return_status, return_status2, return_status3;
        EVENT_BASIC_INFORMATION return_info, return_info2, return_info3;
    } query_test;

    // TODO: Unsure if need to extend tests to Clear/Pulse/Set...
    query_test query_tests[2] = {
        // The list of tests below cannot be rearrange in any way.
        // NotificationEvent
        { .expected_status = STATUS_SUCCESS, .expected_info = { .EventState = TRUE, .EventType = NotificationEvent },
          .expected_status2 = STATUS_SUCCESS, .expected_info2 = { .EventState = FALSE, .EventType = NotificationEvent },
          .expected_status3 = STATUS_SUCCESS, .expected_info3 = { .EventState = 2, .EventType = NotificationEvent } },
        // SynchronizationEvent
        { .expected_status = STATUS_SUCCESS, .expected_info = { .EventState = TRUE, .EventType = SynchronizationEvent },
          .expected_status2 = STATUS_SUCCESS, .expected_info2 = { .EventState = FALSE, .EventType = SynchronizationEvent },
          .expected_status3 = STATUS_SUCCESS, .expected_info3 = { .EventState = 2, .EventType = SynchronizationEvent } }
    };

    for (unsigned i = 0; i < ARRAY_SIZE(query_tests); i++) {
        query_test* test = &query_tests[i];
        // Test #2, with initialize state
        status = NtCreateEvent(&handle,
                               &obj_attr,
                               i, // Use i number for EVENT_TYPE switch
                               TRUE);

        test->return_status = NtQueryEvent(handle, &test->return_info);

        if (NT_SUCCESS(status)) {
            (void)NtClose(handle);
        }

        // Test #3, without initialize state
        status = NtCreateEvent(&handle,
                               &obj_attr,
                               i, // Use i number for EVENT_TYPE switch
                               FALSE);

        test->return_status2 = NtQueryEvent(handle, &test->return_info2);

        if (NT_SUCCESS(status)) {
            (void)NtClose(handle);
        }

        // Test #4, with invalid initialize state
        status = NtCreateEvent(&handle,
                               &obj_attr,
                               i, // Use i number for EVENT_TYPE switch
                               2);

        test->return_status3 = NtQueryEvent(handle, &test->return_info3);

        if (NT_SUCCESS(status)) {
            (void)NtClose(handle);
        }
    }
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_status, expected_status, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info.EventState, expected_info.EventState, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info.EventType, expected_info.EventType, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_status2, expected_status2, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info2.EventState, expected_info2.EventState, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info2.EventType, expected_info2.EventType, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_status3, expected_status3, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info3.EventState, expected_info3.EventState, ARRAY_SIZE(query_tests), "query_tests");
    GEN_CHECK_ARRAY_MEMBER(query_tests, return_info3.EventType, expected_info3.EventType, ARRAY_SIZE(query_tests), "query_tests");

    TEST_END();
}

TEST_FUNC(NtSetEvent)
{
    /* FIXME: This is a stub! implement this function! */
}
