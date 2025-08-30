#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtQueueApcThread)
{
    /* FIXME: This is a stub! implement this function! */
}

typedef struct {
    BOOL run;
    ULONG counter;
    HANDLE hMutex;
} rs_thread_struct;

static DWORD NTAPI NtResumeSuspendThread_thread(void* arg)
{
    rs_thread_struct* rs_thread = (rs_thread_struct*)arg;
    do {
        (void)WaitForSingleObject(rs_thread->hMutex, INFINITE);
        rs_thread->counter++;
        (void)ReleaseMutex(rs_thread->hMutex);
        (void)NtYieldExecution();
    } while(rs_thread->run);
    return 0;
}

#define THREAD_TEST_FAILED() \
    TEST_FAILED(); \
    rs_thread.run = FALSE

static BOOL NtResumeSuspendThreadHybrid(const char* test_name, BOOL suspend)
{
    ASSERT_HEADER;

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "thread");
    (void)WaitForSingleObject(hMutex, INFINITE);

    rs_thread_struct rs_thread = {
        .run = TRUE,
        .counter = 0,
        .hMutex = hMutex
    };
    ULONG old_suspend_count;
    ULONG counter = 0;
    NTSTATUS result;

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

    (void)ReleaseMutex(hMutex);
    (void)NtYieldExecution();

    if (suspend) {
        // Resume the thread
        (void)WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(rs_thread.counter, counter, "counter");
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Resume the already running thread
        // and check the old suspend count is zero
        (void)WaitForSingleObject(hMutex, INFINITE);
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Making sure the thread is still running
        (void)WaitForSingleObject(hMutex, INFINITE);
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        (void)ReleaseMutex(hMutex);
    }
    else {
        // Suspend the running thread
        (void)WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(rs_thread.counter > counter, TRUE, "counter");
        result = NtSuspendThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");
        counter = rs_thread.counter;
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Suspend the suspended thread again for suspend count check
        (void)WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(rs_thread.counter, counter, "counter");
        result = NtSuspendThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Resume the suspend thread but it is remain suspended
        (void)WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(rs_thread.counter, counter, "counter");
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 2, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Resume the suspended thread
        (void)WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(rs_thread.counter, counter, "counter");
        result = NtResumeThread(hThread, &old_suspend_count);
        GEN_CHECK(result, STATUS_SUCCESS, "result");
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        (void)NtYieldExecution();

        // Making sure the thread is still running
        (void)WaitForSingleObject(hMutex, INFINITE);
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        (void)ReleaseMutex(hMutex);
    }

    // End of test, perform the clean up process requirement
    rs_thread.run = FALSE;
    (void)CloseHandle(hThread);
    (void)CloseHandle(hMutex);
    ASSERT_FOOTER(test_name);
}
#undef THREAD_TEST_FAILED

TEST_FUNC(NtResumeThread)
{
    TEST_BEGIN();

    test_passed = NtResumeSuspendThreadHybrid(api_name, FALSE);

    TEST_END();
}

TEST_FUNC(NtSuspendThread)
{
    TEST_BEGIN();

    test_passed = NtResumeSuspendThreadHybrid(api_name, TRUE);

    TEST_END();
}
