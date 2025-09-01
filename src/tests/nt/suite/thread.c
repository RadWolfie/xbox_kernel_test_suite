#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "util/output.h"
#include "assertions/defines.h"

// Create two events for two threads switching
#define THREAD_DUO_EVENTS_CREATE(Primary, Secondary) \
    HANDLE Primary = CreateEventA(NULL, TRUE, TRUE, #Primary); \
    HANDLE Secondary = CreateEventA(NULL, TRUE, FALSE, #Secondary)
// Close event handles for two threads switching
#define THREAD_DUO_EVENTS_DESTROY(Primary, Secondary) \
    (void)CloseHandle(Secondary); \
    (void)CloseHandle(Primary)
// Wait for primary thread event is trigger
#define THREAD_DUO_EVENTS_WAIT_PRIMARY(Primary) \
    WaitForSingleObject(Primary, 1)
// Wait for secondary thread event is trigger
#define THREAD_DUO_EVENTS_WAIT_SECONDARY(Secondary) \
    (void)WaitForSingleObject(Secondary, INFINITE)
// Notify other thread to run (if possible)
#define THREAD_DUO_EVENTS_NOTIFY(Reset, Notify) \
    (void)SetEvent(Notify); \
    (void)ResetEvent(Reset); \
    (void)NtYieldExecution()


TEST_FUNC(NtQueueApcThread)
{
    /* FIXME: This is a stub! implement this function! */
}

typedef struct {
    BOOL run;
    ULONG counter;
    HANDLE hEventThread;
    HANDLE hEventMain;
} rs_thread_struct;

static DWORD NTAPI NtResumeSuspendThread_thread(void* arg)
{
    rs_thread_struct* rs_thread = (rs_thread_struct*)arg;
    do {
        THREAD_DUO_EVENTS_WAIT_SECONDARY(rs_thread->hEventThread);
        rs_thread->counter++;

        // Let other thread to run
        THREAD_DUO_EVENTS_NOTIFY(rs_thread->hEventThread, rs_thread->hEventMain);
    } while(rs_thread->run);
    return 0;
}

#define THREAD_TEST_FAILED() \
    TEST_FAILED(); \
    rs_thread.run = FALSE

static BOOL NtResumeSuspendThreadHybrid(const char* test_name, BOOL suspend)
{
    ASSERT_HEADER;

    THREAD_DUO_EVENTS_CREATE(hEventMain, hEventThread);

    rs_thread_struct rs_thread = {
        .run = TRUE,
        .counter = 0,
        .hEventThread = hEventThread,
        .hEventMain = hEventMain
    };
    ULONG old_suspend_count;
    ULONG counter = 0;
    NTSTATUS result;
    DWORD result_wait;

    if (suspend) {
        result = NtSuspendThread(NULL, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
        result = NtSuspendThread((HANDLE)0xDEADBEEF, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
    }
    else {
        result = NtResumeThread(NULL, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
        result = NtResumeThread((HANDLE)0xDEADBEEF, NULL);
        GEN_CHECK(result, STATUS_INVALID_HANDLE, "result");
    }

    HANDLE hThread = CreateThread(NULL, 0, NtResumeSuspendThread_thread, (void*)&rs_thread, (suspend ? CREATE_SUSPENDED : 0), NULL);
    GEN_CHECK(hThread != NULL, TRUE, "valid handle");
    if(!hThread) {
        print("  ERROR: Did not create thread");
        THREAD_TEST_FAILED();
        ASSERT_FOOTER(test_name);
    }

#if 0
    typedef struct {
        // Previous result check
        BOOL prevThreadCounterAdd;  // 0 or 1
        DWORD prevResultWait;       // WAIT_TIMEOUT or WAIT_OBJECT_0
        // Next result task
        void* nextFuncAPI;          // NtResumeThread or NtSuspendThread
        // don't need individual check result value, it is always STATUS_SUCCESS or
        // STATUS_INVALID_HANDLE for wrong handle input
        ULONG nextFuncSuspendCount; // Return suspend count after call
    } thread_tests;
    for (unsigned i =  0; i < total; i++) {
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        // Can only check previous state begin
        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, (WAIT_TIMEOUT|WAIT_OBJECT_0), "result_wait");

        //counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Can only check previous state end

        result = Nt(Resume|Suspend)Thread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
    }
#endif

    // Let other thread to run (even if it's suspended)
    THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

    if (suspend) {
        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_TIMEOUT, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the thread
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");

        // Let other thread to run
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check running state of the thread
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the already running thread
        // and check the old suspend count is zero
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");

        // Let other thread to run
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Making sure the thread is still running
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
    }
    else {
        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check running state of the thread
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Suspend the running thread
        result = NtSuspendThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");

        // Let other thread to run (which in this case shouldn't be running)
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_TIMEOUT, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Suspend the suspended thread again for suspend count check
        result = NtSuspendThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");

        // Let other thread to run (which in this case shouldn't be running)
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_TIMEOUT, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the suspend thread but it is remain suspended
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 2, "old_suspend_count");

        // Let other thread to run (which in this case shouldn't be running)
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_TIMEOUT, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the suspended thread
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");

        // Let other thread to run
        THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

        result_wait = THREAD_DUO_EVENTS_WAIT_PRIMARY(hEventMain);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Making sure the thread is still running
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
    }

    // End of test, tell the other thread to terminate
    rs_thread.run = FALSE;
    THREAD_DUO_EVENTS_NOTIFY(hEventMain, hEventThread);

    // Perform the clean up process requirement
    (void)CloseHandle(hThread);
    THREAD_DUO_EVENTS_DESTROY(hEventMain, hEventThread);
    ASSERT_FOOTER(test_name);
}
#undef THREAD_TEST_FAILED

TEST_FUNC(NtResumeThread)
{
    TEST_BEGIN();

    for (unsigned i = 0; i< 10; i++) {
        test_passed &= NtResumeSuspendThreadHybrid(api_name, FALSE);
    }

    TEST_END();
}

TEST_FUNC(NtSuspendThread)
{
    TEST_BEGIN();

    for (unsigned i = 0; i< 10; i++) {
        test_passed &= NtResumeSuspendThreadHybrid(api_name, TRUE);
    }

    TEST_END();
}
