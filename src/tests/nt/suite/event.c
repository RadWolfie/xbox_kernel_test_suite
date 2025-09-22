#include <xboxkrnl/xboxkrnl.h>
#include <processthreadsapi.h> // for CreateThread

#include "util/output.h"
#include "util/misc.h"
#include "util/thread.h"
#include "assertions/defines.h"

#define MAX_COUNTER_TOTAL 20

typedef struct {
    ULONG counter1, counter2;
    HANDLE hEventSync1, hEventSync2;
    BOOL passed;
    BOOL terminate;
} thread_sync_counter_s;

typedef struct {
    HANDLE hEvent;
    ULONG counter;
    BOOL completed;
    BOOL terminate;
    DWORD id;
} thread_notify_counter_s;

static DWORD NTAPI NtSetEvent_notify(void* arg)
{
    NTSTATUS status;
    thread_notify_counter_s* thread_data = (thread_notify_counter_s*)arg;
    while(1) {
        // Wait for main thread to be ready
        while(thread_data->completed) {
            (void)NtYieldExecution();
        }
        THREAD_DUO_EVENTS_WAIT(thread_data, thread_data->hEvent, status);

        // End of thread test completion, setting up for the next counter test
        thread_data->counter++;
        thread_data->completed = TRUE;
    }
    thread_data->terminate = TRUE;
    return 0;
}

static DWORD NTAPI NtSetEvent_sync1(void* arg)
{
    thread_sync_counter_s* thread_data = (thread_sync_counter_s*)arg;
    NTSTATUS status;
    do {
        THREAD_DUO_EVENTS_WAIT(thread_data, thread_data->hEventSync1, status);

        thread_data->passed &= (thread_data->counter1 == thread_data->counter2);

        thread_data->counter1++;

        // Let other thread to run
        THREAD_DUO_EVENTS_SET(thread_data->hEventSync2);
    } while(thread_data->counter1 < MAX_COUNTER_TOTAL);

    thread_data->terminate = TRUE;
    return 0;
}

static DWORD NTAPI NtSetEvent_sync2(void* arg)
{
    thread_sync_counter_s* thread_data = (thread_sync_counter_s*)arg;
    NTSTATUS status;
    do {
        THREAD_DUO_EVENTS_WAIT(thread_data, thread_data->hEventSync2, status);

        thread_data->counter2++;

        thread_data->passed &= (thread_data->counter1 == thread_data->counter2);

        // Let other thread to run
        THREAD_DUO_EVENTS_SET(thread_data->hEventSync1);
    } while(thread_data->counter2 < MAX_COUNTER_TOTAL);
    thread_data->terminate = TRUE;
    return 0;
}

TEST_FUNC(NtClearEvent)
{
    TEST_BEGIN();

    HANDLE handle;
    PVOID object;
    NTSTATUS status;
    ANSI_STRING obj_name;

    // Test #1, test invalid handles
    EVENT_BASIC_INFORMATION info = { 0 };
    status = NtClearEvent(NULL);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");
    status = NtClearEvent((HANDLE)0xBEEF);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");

    RtlInitAnsiString(&obj_name, TEST_GET_API_NAME);
    OBJECT_ATTRIBUTES obj_attr;
    obj_attr.ObjectName = &obj_name;
    InitializeObjectAttributes(&obj_attr, &obj_name, 0, ObWin32NamedObjectsDirectory(), NULL);

    typedef struct {
        EVENT_TYPE input_event_type; // NotificationEvent or SynchronizationEvent
        BOOLEAN input_init_state;
        NTSTATUS expected_status, expected_status2, expected_status3;
        NTSTATUS return_status, return_status2, return_status3;
    } clear_test;
    clear_test clear_tests[] = {
        // NotificationEvent
        { .input_event_type = NotificationEvent, .input_init_state = FALSE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS },
        { .input_event_type = NotificationEvent, .input_init_state = TRUE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS },
        // SynchronizationEvent
        { .input_event_type = SynchronizationEvent, .input_init_state = FALSE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS },
        { .input_event_type = SynchronizationEvent, .input_init_state = TRUE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(clear_tests); i++) {
        clear_test* test = &clear_tests[i];
        // Test #2, verify creation event is initialize or not
        (void)NtCreateEvent(&handle,
                            &obj_attr,
                            test->input_event_type,
                            test->input_init_state);

        test->return_status = NtClearEvent(handle);
        test->return_status2 = NtClearEvent(handle);

        (void)NtClose(handle);
    }
    GEN_CHECK_ARRAY_MEMBER(clear_tests, return_status, expected_status, ARRAY_SIZE(clear_tests), "clear_tests");
    GEN_CHECK_ARRAY_MEMBER(clear_tests, return_status2, expected_status2, ARRAY_SIZE(clear_tests), "clear_tests");

    TEST_END();
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
    // Maybe could be implement...
    // What it does is ...
    // Pulse the event: sets it signaled and then immediately non-signaled
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
    TEST_BEGIN();

    HANDLE handle;
    PVOID object;
    NTSTATUS status;
    ANSI_STRING obj_name;

    // Test #1, test invalid handles
    LONG previous_state = -1;
    status = NtSetEvent(NULL, &previous_state);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");
    GEN_CHECK(previous_state, -1, "previous_state");
    status = NtSetEvent((HANDLE)0xBEEF, &previous_state);
    GEN_CHECK(status, STATUS_INVALID_HANDLE, "status");
    GEN_CHECK(previous_state, -1, "previous_state");

    RtlInitAnsiString(&obj_name, TEST_GET_API_NAME);
    OBJECT_ATTRIBUTES obj_attr;
    obj_attr.ObjectName = &obj_name;
    InitializeObjectAttributes(&obj_attr, &obj_name, 0, ObWin32NamedObjectsDirectory(), NULL);

    typedef struct {
        EVENT_TYPE input_event_type; // NotificationEvent or SynchronizationEvent
        BOOLEAN input_init_state;
        LONG expected_prev_state, expected_prev_state2, expected_prev_state3, expected_prev_state4;
        NTSTATUS expected_status, expected_status2, expected_status3, expected_status4;
        LONG return_prev_state, return_prev_state2, return_prev_state3, return_prev_state4;
        NTSTATUS return_status, return_status2, return_status3, return_status4;
    } set_single_test;
    set_single_test set_single_tests[] = {
        // NotificationEvent
        { .input_event_type = NotificationEvent, .input_init_state = FALSE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS, .expected_status3 = STATUS_SUCCESS,
          .expected_prev_state = FALSE, .expected_prev_state2 = FALSE,
          .expected_prev_state3 = TRUE, .expected_prev_state4 = FALSE },
        { .input_event_type = NotificationEvent, .input_init_state = TRUE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS, .expected_status3 = STATUS_SUCCESS,
          .expected_prev_state = TRUE, .expected_prev_state2 = FALSE,
          .expected_prev_state3 = TRUE, .expected_prev_state4 = FALSE },
        // SynchronizationEvent
        { .input_event_type = SynchronizationEvent, .input_init_state = FALSE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS, .expected_status3 = STATUS_SUCCESS,
          .expected_prev_state = FALSE, .expected_prev_state2 = FALSE,
          .expected_prev_state3 = TRUE, .expected_prev_state4 = FALSE },
        { .input_event_type = SynchronizationEvent, .input_init_state = TRUE,
          .expected_status = STATUS_SUCCESS, .expected_status2 = STATUS_SUCCESS, .expected_status3 = STATUS_SUCCESS,
          .expected_prev_state = TRUE , .expected_prev_state2 = FALSE,
          .expected_prev_state3 =  TRUE , .expected_prev_state4 = FALSE },
    };
    for (unsigned i = 0; i < ARRAY_SIZE(set_single_tests); i++) {
        set_single_test* test = &set_single_tests[i];
        // Test #2, verify creation event is initialize or not
        (void)NtCreateEvent(&handle,
                            &obj_attr,
                            test->input_event_type,
                            test->input_init_state);
        test->return_status = NtSetEvent(handle, &test->return_prev_state);
        (void)NtClearEvent(handle);

        // Test #3, verify if event is (re-)initialize
        test->return_status2 = NtSetEvent(handle, &test->return_prev_state2);

        // Test #4, verify if event is remain initialize
        test->return_status3 = NtSetEvent(handle, &test->return_prev_state3);

        // Test #5, verify if event is initialize again
        (void)NtClearEvent(handle);
        test->return_status4 = NtSetEvent(handle, &test->return_prev_state4);

        (void)NtClose(handle);
    }
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_status, expected_status, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_status2, expected_status2, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_status3, expected_status3, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_status4, expected_status4, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_prev_state, expected_prev_state, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_prev_state2, expected_prev_state2, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_prev_state3, expected_prev_state3, ARRAY_SIZE(set_single_tests), "set_single_tests");
    GEN_CHECK_ARRAY_MEMBER(set_single_tests, return_prev_state4, expected_prev_state4, ARRAY_SIZE(set_single_tests), "set_single_tests");

    // Test #6 - SynchronizationEvent test (aka without manual reset event state)
    // Since we already have THREAD_DUO_EVENTS wrapper which use the SynchronizationEvent method
    HANDLE event1, event2;
    THREAD_DUO_EVENTS_CREATE(event1, event2, TRUE);
    thread_sync_counter_s thread_data = {
        .counter1 = 0,
        .counter2 = 0,
        .hEventSync1 = event1,
        .hEventSync2 = event2,
        .passed = TRUE,
        .terminate = FALSE
    };
    HANDLE thread1 = CreateThread(NULL, 0, NtSetEvent_sync1, (void*)&thread_data, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, NtSetEvent_sync2, (void*)&thread_data, 0, NULL);
    GEN_CHECK(thread1 != NULL, TRUE, "valid handle (thread1)");
    GEN_CHECK(thread2 != NULL, TRUE, "valid handle (thread2)");
    if (!thread1 || !thread2) {
        if (thread1) {
            (void)NtClose(thread1);
        }
        if (thread2) {
            (void)NtClose(thread2);
        }
        THREAD_DUO_EVENTS_DESTROY(event1, event2);
        TEST_END();
    }

    while(!thread_data.terminate) {
        (void)NtYieldExecution();
    }
    // Wait for duo event's threads to be done testing
    // then clean up threads and handles
    THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(thread1);
    THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(thread2);
    THREAD_DUO_EVENTS_DESTROY(event1, event2);

    // Verify test results
    GEN_CHECK(thread_data.passed, TRUE, "passed");
    GEN_CHECK(thread_data.counter1, MAX_COUNTER_TOTAL, "counter1");
    GEN_CHECK(thread_data.counter2, MAX_COUNTER_TOTAL, "counter2");

    // Test #7 - NotificationEvent test (aka with manual reset event state)
    ULONG counter = 0;
    status = NtCreateEvent(&event1, &obj_attr, NotificationEvent, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "status");

    thread_notify_counter_s thread1_data = {
        .completed = FALSE,
        .counter = 0,
        .hEvent = event1,
        .id = 1
    };
    thread_notify_counter_s thread2_data = {
        .completed = FALSE,
        .counter = 0,
        .hEvent = event1,
        .id = 2
    };
    thread1 = CreateThread(NULL, 0, NtSetEvent_notify, (void*)&thread1_data, 0, NULL);
    thread2 = CreateThread(NULL, 0, NtSetEvent_notify, (void*)&thread2_data, 0, NULL);
    GEN_CHECK(thread1 != NULL, TRUE, "valid handle (thread1)");
    GEN_CHECK(thread2 != NULL, TRUE, "valid handle (thread2)");
    if (!thread1 || !thread2) {
        if (thread1) {
            (void)NtClose(thread1);
        }
        if (thread2) {
            (void)NtClose(thread2);
        }
        THREAD_DUO_EVENTS_DESTROY(event1, event2);
        TEST_END();
    }

    do {
        status = NtSetEvent(event1, NULL);
        if (!NT_SUCCESS(status)) {
            break;
        }
        while(!thread1_data.completed || !thread2_data.completed) {
            if (thread1_data.terminate || thread2_data.terminate) {
                TEST_FAILED();
                break;
            }
            (void)NtYieldExecution();
        }
        (void)NtClearEvent(event1);
        if (thread1_data.terminate || thread2_data.terminate) {
            break;
        }
        thread1_data.completed = FALSE;
        thread2_data.completed = FALSE;
        counter++;
    } while (counter < MAX_COUNTER_TOTAL);

    // Close event handle will cause threads to continue and terminate themselves
    thread1_data.terminate = TRUE;
    thread2_data.terminate = TRUE;
    (void)NtClose(event1);

    // Wait for sync threads to be terminated
    THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(thread1);
    THREAD_DUO_EVENTS_DESTROY_THREAD_WAIT(thread2);

    // Verify the tests
    GEN_CHECK(thread1_data.counter, counter, "thread1_data.counter");
    GEN_CHECK(thread2_data.counter, counter, "thread2_data.counter");
    GEN_CHECK(counter, MAX_COUNTER_TOTAL, "counter");

    TEST_END();
}
