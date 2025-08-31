#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(KeAlertResumeThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeAlertThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeBoostPriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeDelayExecutionThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeGetCurrentThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeQueryBasePriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

typedef struct {
    BOOL run;
    ULONG counter;
    HANDLE hEventThread;
    HANDLE hMutex;
} rs_thread_struct;

#define THREAD_TEST_FAILED() \
    TEST_FAILED(); \
    rs_thread.run = FALSE

static DWORD NTAPI KeResumeSuspendThread_thread(void* arg)
{
    BOOL test_passed = 1;
    rs_thread_struct* rs_thread = (rs_thread_struct*)arg;
    do {
        (void)WaitForSingleObject(rs_thread->hEventThread, INFINITE);
        DWORD result_wait = WaitForSingleObject(rs_thread->hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait (thread)");
        rs_thread->counter++;
        (void)ReleaseMutex(rs_thread->hMutex);

        (void)NtYieldExecution();
    } while(rs_thread->run);
    return 0;
}

static BOOL KeResumeSuspendThreadHybrid(const char* test_name, BOOL suspend)
{
    ASSERT_HEADER;

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "mutex");
    HANDLE hEventThread = CreateEventA(NULL, FALSE, FALSE, "event");
    (void)WaitForSingleObject(hMutex, INFINITE);

    rs_thread_struct rs_thread = {
        .run = TRUE,
        .counter = 0,
        .hEventThread = hEventThread,
        .hMutex = hMutex
    };
    ULONG old_suspend_count;
    ULONG counter = 0;
    NTSTATUS result;
    DWORD result_wait;

#ifdef DISABLED
    // TODO: Could not catch invalid argument at this time.
    if (suspend) {
        BOOL catch = -1;
        __try {
            (void)KeSuspendThread(NULL);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
        catch = -1;
        __try {
            (void)KeSuspendThread((PKTHREAD)0xBEEF);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
    }
    else {
        BOOL catch = -1;
        __try {
            (void)KeResumeThread(NULL);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
        catch = -1;
        __try {
            (void)KeResumeThread((PKTHREAD)0xBEEF);
            catch = 0;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            catch = 1;
        }
        GEN_CHECK(catch, 1, "catch");
    }
#endif

    HANDLE hThread = CreateThread(NULL, 0, KeResumeSuspendThread_thread, (void*)&rs_thread, (suspend ? CREATE_SUSPENDED : 0), NULL);
    GEN_CHECK(hThread != NULL, TRUE, "valid handle");
    if(!hThread) {
        print("  ERROR: Did not create thread");
        THREAD_TEST_FAILED();
        ASSERT_FOOTER(test_name);
    }
    // Since we are checking Ke functions, we need the actual thread object.
    PETHREAD Thread;
    result = ObReferenceObjectByHandle(hThread, &PsThreadObjectType, (PVOID*)&Thread);
    GEN_CHECK(NT_SUCCESS(result), TRUE, "result");

    (void)ReleaseMutex(hMutex);
    // Let other thread to run
    (void)SetEvent(hEventThread);
    (void)NtYieldExecution();

    if (suspend) {

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the thread
        old_suspend_count = KeResumeThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check running state of the thread
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the already running thread
        // and check the old suspend count is zero
        old_suspend_count = KeResumeThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Making sure the thread is still running
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");

        (void)ReleaseMutex(hMutex);
    }
    else {
        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check running state of the thread
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Suspend the running thread
        old_suspend_count = KeSuspendThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 0, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run (which in this case shouldn't be running)
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Suspend the suspended thread again for suspend count check
        old_suspend_count = KeSuspendThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run (which in this case shouldn't be running)
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the suspend thread but it is remain suspended
        old_suspend_count = KeResumeThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 2, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run (which in this case shouldn't be running)
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Check suspended state of the thread
        GEN_CHECK(rs_thread.counter, counter, "counter");
        // Resume the suspended thread
        old_suspend_count = KeResumeThread(&Thread->Tcb);
        GEN_CHECK(old_suspend_count, 1, "old_suspend_count");
        (void)ReleaseMutex(hMutex);

        // Let other thread to run
        (void)SetEvent(hEventThread);
        (void)NtYieldExecution();

        result_wait = WaitForSingleObject(hMutex, INFINITE);
        GEN_CHECK(result_wait, WAIT_OBJECT_0, "result_wait");
        // Making sure the thread is still running
        counter++;
        GEN_CHECK(rs_thread.counter, counter, "counter");

        (void)ReleaseMutex(hMutex);
    }

    // End of test, tell the other thread to terminate
    rs_thread.run = FALSE;
    (void)SetEvent(hEventThread);
    (void)NtYieldExecution();

    // Perform the clean up process requirement
    ObfDereferenceObject(Thread);
    (void)CloseHandle(hThread);
    (void)CloseHandle(hEventThread);
    (void)CloseHandle(hMutex);
    ASSERT_FOOTER(test_name);
}
#undef THREAD_TEST_FAILED

TEST_FUNC(KeResumeThread)
{
    TEST_BEGIN();

    for (unsigned i = 0; i< 10; i++) {
        test_passed &= KeResumeSuspendThreadHybrid(api_name, FALSE);
    }

    TEST_END();
}

TEST_FUNC(KeSetBasePriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSetDisableBoostThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSetPriorityThread)
{
    /* FIXME: This is a stub! implement this function! */
}

TEST_FUNC(KeSuspendThread)
{
    TEST_BEGIN();

    for (unsigned i = 0; i< 10; i++) {
        test_passed &= KeResumeSuspendThreadHybrid(api_name, TRUE);
    }

    TEST_END();
}

TEST_FUNC(KeTestAlertThread)
{
    /* FIXME: This is a stub! implement this function! */
}
