#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "assertion_defines.h"
#include "ke_assertions.h"
#include "output.h"

void test_KeAlertResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeAlertThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBoostPriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBugCheck(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeBugCheckEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeCancelTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeConnectInterrupt(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeDelayExecutionThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeDisconnectInterrupt(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeEnterCriticalRegion(){
    const char* func_num = "0x0065";
    const char* func_name = "KeEnterCriticalRegion";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    tests_passed &= assert_critical_region(thread, 0, "Before entering critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "In critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -2, "Entered critical region again");

    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region twice");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_KeGetCurrentIrql(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeGetCurrentThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeApc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeEvent(){
    /* FIXME: This is a stub! implement this function! */
}

// Testing Ke's Interrupt process...
static KINTERRUPT InterruptObject;
static KDPC GPU_Dpc;
static BOOL DpcFunc_called;
static void __stdcall GPU_DpcFunc(
	IN KDPC* Dpc,
	IN PVOID DeferredContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
)
{
    if (!DpcFunc_called) {
        DpcFunc_called = 1;
    }
#if 0
    static BOOL show_message = 1;
    if (show_message) {
        print("  Hello from GPU_DpcFunc call!");
        show_message = 0;
    }
#endif

#if 0
    //static unsigned counter = 1;
    //print("  GPU_DpcFunc: counter=%u", counter);
    //counter++;
#endif
}

static BOOL IsrFunc_called;
static BOOLEAN __stdcall GPU_InterruptServiceRountine(
	IN KINTERRUPT* Interrupt,
	IN PVOID ServiceContext
)
{
    if (!IsrFunc_called) {
        IsrFunc_called = 1;
    }
#if 1
    static BOOL show_message = 1;
    if (show_message) {
        print("  Hello from GPU_InterruptServiceRountine call!");
        ULONG ret = KeDisconnectInterrupt(Interrupt);
        print("  GPU_InterruptServiceRountine: KeDisconnectInterrupt return %x", ret);
        KeInsertQueueDpc(&GPU_Dpc, NULL, NULL);
        show_message = 0;
    }
#endif

#if 0
    static unsigned counter = 1;
    print("  GPU_InterruptServiceRountine: counter=%u", counter);
    counter++;
#endif

    return TRUE;
}

void test_KeInitializeInterrupt(){
    const char* func_num = "0x006D";
    const char* func_name = "KeInitializeInterrupt";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    KIRQL irql;
    ULONG irq_test = 7;
    print("DEBUG: irq_test=%u", irq_test);
    ULONG InterruptVector = HalGetInterruptVector(irq_test, &irql);

    // We may not need Dpc rountine since minimal requirement is Isr
    KeInitializeDpc(&GPU_Dpc, GPU_DpcFunc, NULL);
    //RtlZeroMemory(&InterruptObject, sizeof(KINTERRUPT));

    KeInitializeInterrupt(&InterruptObject, GPU_InterruptServiceRountine, NULL, InterruptVector, irql, LevelSensitive, TRUE);
    
    // Inspect each member variables are correctly setup.
    GEN_CHECK(InterruptObject.ServiceRoutine, GPU_InterruptServiceRountine, "InterruptObject.ServiceRoutine");
    GEN_CHECK(InterruptObject.ServiceContext, NULL, "InterruptObject.ServiceContext");
    //print("DEBUG: InterruptObject.BusInterruptLevel=%u", InterruptObject.BusInterruptLevel);
    GEN_CHECK(InterruptObject.BusInterruptLevel, (InterruptVector-0x30), "InterruptObject.BusInterruptLevel");
    GEN_CHECK(InterruptObject.Irql, irql, "InterruptObject.Irql");
    GEN_CHECK(InterruptObject.Connected, FALSE, "InterruptObject.Connected");
    GEN_CHECK(InterruptObject.ShareVector, TRUE, "InterruptObject.ShareVector"); // Not set??
    //print("DEBUG: InterruptObject.Mode=%llu", InterruptObject.Mode);
    GEN_CHECK(InterruptObject.Mode, LevelSensitive, "InterruptObject.Mode");
    print("DEBUG: InterruptObject.ServiceCount=%u", InterruptObject.ServiceCount);
    //GEN_CHECK(InterruptObject.ServiceCount, , "InterruptObject.ServiceCount"); // Not set

    // TODO: Fix the comment for "conected" to "connected" typo.
    if (!KeConnectInterrupt(&InterruptObject)) {
        print("ERROR: Unable to connect interrupt");
        test_passed = 0;
    }
    else {
        print("SUCCESS: Able to connect interrupt");
    }

#if 1
    KIRQL old_irql = KfRaiseIrql(irql);
    print("DEBUG: called KfRaiseIrql");
    KfLowerIrql(old_irql);
    print("DEBUG: called KfLowerIrql");
#endif

    Sleep(500);

    if (IsrFunc_called) {
        print("DEBUG: IsrFunc been called");
    }
    else {
        print("DEBUG: IsrFunc didn't get called");
    }

    if (DpcFunc_called) {
        print("DEBUG: DpcFunc been called");
    }
    else {
        print("DEBUG: DpcFunc didn't get called");
    }

    // TODO: Add comment for what return should be expected.
    if (InterruptObject.Connected) {
        BOOLEAN wasConnected = KeDisconnectInterrupt(&InterruptObject);
        GEN_CHECK(wasConnected, TRUE, "wasConnected1");
    }
    else {
        test_passed = 0;
    }

    BOOLEAN wasConnected = KeDisconnectInterrupt(&InterruptObject);
    GEN_CHECK(wasConnected, FALSE, "wasConnected2");

    BOOLEAN testBool2 = KeRemoveQueueDpc(&GPU_Dpc);

    print_test_footer(func_num, func_name, test_passed);
}

void test_KeInitializeMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInitializeTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertByKeyDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertHeadQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueueApc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInsertQueueDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeInterruptTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeIsExecutingDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeLeaveCriticalRegion(){
    const char* func_num = "0x007A";
    const char* func_name = "KeLeaveCriticalRegion";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    KeEnterCriticalRegion();
    KeEnterCriticalRegion();

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "Leave critical region after entering twice");

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region again");

    print_test_footer(func_num, func_name, tests_passed);
}

void test_KePulseEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryBasePriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryInterruptTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryPerformanceCounter(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQueryPerformanceFrequency(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeQuerySystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRaiseIrqlToDpcLevel(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRaiseIrqlToSynchLevel(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeReleaseMutant(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeReleaseSemaphore(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveByKeyDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveEntryDeviceQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRemoveQueueDpc(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeResetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRestoreFloatingPointState(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeResumeThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeRundownQueue(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSaveFloatingPointState(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetBasePriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetDisableBoostThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetEvent(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetEventBoostPriority(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetPriorityProcess(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetPriorityThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetTimer(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSetTimerEx(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeStallExecutionProcessor(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSuspendThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSynchronizeExecution(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeSystemTime(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTestAlertThread(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTickCount(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeTimeIncrement(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeWaitForMultipleObjects(){
    /* FIXME: This is a stub! implement this function! */
}

void test_KeWaitForSingleObject(){
    /* FIXME: This is a stub! implement this function! */
}
