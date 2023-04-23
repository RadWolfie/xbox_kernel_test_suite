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

static KDPC Ke_Dpc;
static BOOL DpcFunc_called;
static void __stdcall Ke_DpcFunc(
	IN KDPC* Dpc,
	IN PVOID DeferredContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
)
{
    if (!DpcFunc_called) {
        DpcFunc_called = 1;
    }
#if 1
    static BOOL show_message = 1;
    if (show_message) {
        //print("  Hello from Ke_DpcFunc call!");
        //print("  Ke_DpcFunc: KeDisconnectInterrupt return %x", ret);
        show_message = 0;
    }
#endif

#if 0
    //static unsigned counter = 1;
    //print("  Ke_DpcFunc: counter=%u", counter);
    //counter++;
#endif
}

void test_KeInitializeDpc(){
    const char* func_num = "0x006B";
    const char* func_name = "KeInitializeDpc";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);
#if 1 // Moving stuff here

    // Force set these members to verify if KeInitializeDpc call does set them or not.
    // According to hardware, they are not set. Let's force check them anyway.
    Ke_Dpc.SystemArgument1 = (PVOID)0x15;
    Ke_Dpc.SystemArgument2 = (PVOID)0x25;

    KeInitializeDpc(&Ke_Dpc, Ke_DpcFunc, &DpcFunc_called);

    // Inspect each member variables are correctly setup.
    GEN_CHECK(Ke_Dpc.Type, 0x13/*DpcObject*/, "Ke_Dpc.Type");
    GEN_CHECK(Ke_Dpc.Inserted, FALSE, "Ke_Dpc.Inserted");
    //GEN_CHECK(Ke_Dpc.DpcListEntry, , "Ke_Dpc.DpcListEntry");
    print("DEBUG: Ke_Dpc.DpcListEntry=%p, Blink=%p, Flink=%p", &Ke_Dpc.DpcListEntry, Ke_Dpc.DpcListEntry.Blink, Ke_Dpc.DpcListEntry.Flink);
    GEN_CHECK(Ke_Dpc.DeferredRoutine, Ke_DpcFunc, "Ke_Dpc.DeferredRoutine");
    GEN_CHECK(Ke_Dpc.DeferredContext, &DpcFunc_called, "Ke_Dpc.DeferredContext");
    GEN_CHECK(Ke_Dpc.SystemArgument1, (PVOID)0x15, "Ke_Dpc.SystemArgument1");
    GEN_CHECK(Ke_Dpc.SystemArgument2, (PVOID)0x25, "Ke_Dpc.SystemArgument2");

    KeInsertQueueDpc(&Ke_Dpc, NULL, NULL);

    // TODO: Move below into different test.
    GEN_CHECK(Ke_Dpc.Inserted, TRUE, "Ke_Dpc.Inserted"); // TODO: Why is this says false after insert call above?
    print("DEBUG: Ke_Dpc.DpcListEntry=%p, Blink=%p, Flink=%p", &Ke_Dpc.DpcListEntry, Ke_Dpc.DpcListEntry.Blink, Ke_Dpc.DpcListEntry.Flink);
    GEN_CHECK(Ke_Dpc.SystemArgument1, NULL, "Ke_Dpc.SystemArgument1");
    GEN_CHECK(Ke_Dpc.SystemArgument2, NULL, "Ke_Dpc.SystemArgument2");

    // TODO: Move couple lines below into KeRemoveQueueDpc test and find out why it return false here...
    BOOLEAN removal = KeRemoveQueueDpc(&Ke_Dpc);
    GEN_CHECK(removal, TRUE, "KeRemoveQueueDpc return");

    //call dispatcher function to start process I hope
    HalRequestSoftwareInterrupt(2); //=DISPATCH_LEVEL

    if (DpcFunc_called) {
        print("DEBUG: DpcFunc been called");
    }
    else {
        print("DEBUG: DpcFunc didn't get called");
        test_passed = 0;
    }

    // TODO: Move couple lines below into KeRemoveQueueDpc test
    removal = KeRemoveQueueDpc(&Ke_Dpc);
    GEN_CHECK(removal, FALSE, "KeRemoveQueueDpc return");
#endif

    print_test_footer(func_num, func_name, test_passed);
}

void test_KeInitializeEvent(){
    /* FIXME: This is a stub! implement this function! */
}

// Testing Ke's Interrupt process...
static KINTERRUPT InterruptObject;
// 1 =USB0 | seems connected but locked up
// 2       | (used for cascade) is connected but no response
// 3 =GPU  | fail to connect interrupt.
// 4 =NIC  | success connected but doesn't trigger ISR function
// 5 =APU  | success connected but doesn't trigger ISR function
// 6 =ACI  | success connected but doesn't trigger ISR function
// 7       | (spurious) success connected but stuck in ISR loophell; yet if with Dpc callback, locked up; doesn't let it continue for some reason..
// 8       | fail to connect interrupt.
// 9 =USB1 | success connected but doesn't trigger ISR function
// 10      | success connected but stuck in ISR loophell
// 11      | fail to connect interrupt.
// 12      | fail to connect interrupt.
// 13      | success connect but ISR didn't get called.
// 14=IDE  | is unable to connect interrupt.
// 15      | (spurious) success connect and ISR called.
// 16 - 26 | (doesn't exists) fail to connect interrupt.
static ULONG irq_test = 7;

#if 0 // Try attempt trigger network device's interrupt
#define NvRegIrqStatus 0x000
#define NvRegIrqMask 0x004
#define NvRegMIIStatus 0x180
#define BASE ((void *)0xFEF00000)
#define reg32(offset) (*((volatile uint32_t *)((uintptr_t)BASE + (offset))))
#endif

static BOOL IsrFunc_called1;
static BOOL IsrFunc_called2;
static BOOLEAN __stdcall Ke_InterruptServiceRountine(
	IN KINTERRUPT* Interrupt,
	IN PVOID ServiceContext
)
{
    if (!IsrFunc_called1) {
        IsrFunc_called1 = 1;
    }
#if 1
    static BOOL show_message = 1;
    if (show_message) {
        print("  Hello from Ke_InterruptServiceRountine call!"); // TODO: remove this line...
        HalDisableSystemInterrupt(Interrupt->BusInterruptLevel); // this is what works for get out of loophell but is only a temporary workaround
        show_message = 0;

        return TRUE;
    }
#endif

    if (!IsrFunc_called2) {
        IsrFunc_called2 = 1;
    }
    print("  Hello from Ke_InterruptServiceRountine call again!"); // TODO: remove this line...
    HalDisableSystemInterrupt(Interrupt->BusInterruptLevel); // this is what works for get out of loophell but is only a temporary workaround

    return TRUE;
}

// TODO: Try figure out how to trigger interrupt from irq_test that are from either...:
// 1) loophell IRQs (require restore how it was before) or
// 2) we manually trigger the interrupt in somehow?
void test_KeInitializeInterrupt(){
    const char* func_num = "0x006D";
    const char* func_name = "KeInitializeInterrupt";
    BOOL test_passed = 1;
    print_test_header(func_num, func_name);

    KIRQL irql;
    print("DEBUG: irq_test=%u", irq_test);
    ULONG InterruptVector = HalGetInterruptVector(irq_test, &irql);

    //RtlZeroMemory(&InterruptObject, sizeof(KINTERRUPT));

    // Force set these members to verify if KeInitializeInterrupt call does set them or not.
    // According to hardware, they are not set. Let's force check them anyway.
    InterruptObject.ShareVector = 0x15;
    InterruptObject.ServiceCount = 0x25;

    KeInitializeInterrupt(&InterruptObject, Ke_InterruptServiceRountine, &irq_test, InterruptVector, irql, LevelSensitive, TRUE);

    // Inspect each member variables are correctly setup.
    GEN_CHECK(InterruptObject.ServiceRoutine, Ke_InterruptServiceRountine, "InterruptObject.ServiceRoutine");
    GEN_CHECK(InterruptObject.ServiceContext, &irq_test, "InterruptObject.ServiceContext");
    //print("DEBUG: InterruptObject.BusInterruptLevel=%u", InterruptObject.BusInterruptLevel);
    GEN_CHECK(InterruptObject.BusInterruptLevel, (InterruptVector-0x30), "InterruptObject.BusInterruptLevel");
    GEN_CHECK(InterruptObject.Irql, irql, "InterruptObject.Irql");
    GEN_CHECK(InterruptObject.Connected, FALSE, "InterruptObject.Connected");
    GEN_CHECK(InterruptObject.ShareVector, 0x15, "InterruptObject.ShareVector"); // Not set
    //print("DEBUG: InterruptObject.Mode=%llu", InterruptObject.Mode);
    GEN_CHECK(InterruptObject.Mode, LevelSensitive, "InterruptObject.Mode");
    GEN_CHECK(InterruptObject.ServiceCount, 0x25, "InterruptObject.ServiceCount"); // Not set
    //TODO: How should we decode DispatchCode member variable?
    // InterruptObject.DispatchCode 

    // TODO: Fix the comment for "conected" to "connected" typo.
    if (!KeConnectInterrupt(&InterruptObject)) {
        print("ERROR: Unable to connect interrupt");
        test_passed = 0;
    }
    else {
        print("SUCCESS: Able to connect interrupt");
#if 0 // Try attempt trigger network device's interrupt
        reg32(NvRegIrqMask) = 0xFF;
        uint32_t nv_irq = reg32(NvRegIrqStatus);
        print("DEBUG: NvRegIrqStatus=%x", nv_irq);
        uint32_t nv_mii = reg32(NvRegMIIStatus);
        print("DEBUG: NvRegMIIStatus=%x", nv_mii);
        reg32(NvRegIrqStatus) = 0x0040; //=NVREG_IRQ_LINK
#endif
    }

    // might not be the right way... idk
    //HalRequestSoftwareInterrupt(irql);
#if 0
    KIRQL old_irql = KfRaiseIrql(irql);
    print("DEBUG: called KfRaiseIrql");
    KfLowerIrql(old_irql);
    print("DEBUG: called KfLowerIrql");
#endif

    HalEnableSystemInterrupt(InterruptObject.BusInterruptLevel, InterruptObject.Mode);

    Sleep(500);

    if (IsrFunc_called1) {
        print("DEBUG: IsrFunc x1 been called");
    }
    else {
        print("DEBUG: IsrFunc x1 didn't get called");
        test_passed = 0;
    }

    if (IsrFunc_called2) {
        print("DEBUG: IsrFunc x2 been called");
    }
    else {
        print("DEBUG: IsrFunc x2 didn't get called");
        test_passed = 0;
    }

    GEN_CHECK(InterruptObject.Connected, TRUE, "InterruptObject.Connected");
    // TODO: Add comment for what return should be expected.
    if (InterruptObject.Connected) {
        BOOLEAN wasConnected = KeDisconnectInterrupt(&InterruptObject);
        GEN_CHECK(wasConnected, TRUE, "wasConnected1");
    }

    BOOLEAN wasConnected = KeDisconnectInterrupt(&InterruptObject);
    GEN_CHECK(wasConnected, FALSE, "wasConnected2");

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
