#include "ex_assertions.h"
#include "assertion_defines.h"

BOOL assert_ERWLOCK_equals(
    PERWLOCK ReadWriteLock,
    LONG expected_LockCount,
    ULONG expected_WritersWaitingCount,
    ULONG expected_ReadersWaitingCount,
    ULONG expected_ReadersEntryCount,
    const char* test_name
) {
    ASSERT_HEADER

    GEN_CHECK(ReadWriteLock->LockCount, expected_LockCount, "LockCount")
    GEN_CHECK(ReadWriteLock->WritersWaitingCount, expected_WritersWaitingCount, "WritersWaitingCount")
    GEN_CHECK(ReadWriteLock->ReadersWaitingCount, expected_ReadersWaitingCount, "ReadersWaitingCount")
    GEN_CHECK(ReadWriteLock->ReadersEntryCount, expected_ReadersEntryCount, "ReadersEntryCount")

    ASSERT_FOOTER(test_name)
}

static LONG NTAPI assert_UnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo) {
    print("  Test %s catch", __func__);
    return EXCEPTION_EXECUTE_HANDLER;
}

int assert_ExceptionHandlerCatcher(ExceptionHandlerCatcherParams* ehc_params,
                                   void* func_exception,
                                   DWORD ExceptionCode,
                                   PEXCEPTION_POINTERS ExceptionInformation) {
    BOOL test_passed = 1;
    // NOTE: Currently, nxdk doesn't have support for unhandled exception filter calls.
//#define RESTORE_EXCEPT_FILTER 1
#ifdef RESTORE_EXCEPT_FILTER
    PTOP_LEVEL_EXCEPTION_FILTER restore_exception_filter;
    restore_exception_filter = SetUnhandledExceptionFilter(test_iUnhandledExceptionFilter);

    UnhandledExceptionFilter(ExceptionInformation);

    // Now restore to previous exception filter.
    SetUnhandledExceptionFilter(restore_exception_filter);
#endif

    PEXCEPTION_RECORD pExceptionRecord = ExceptionInformation->ExceptionRecord;
    PCONTEXT pContextRecord = ExceptionInformation->ContextRecord;

    // Force modify to skip trigger raise again.
    if (ehc_params->ExceptionHandlerReturn == EXCEPTION_CONTINUE_EXECUTION) {
        // Force set to allow continue execution.
        pExceptionRecord->ExceptionFlags = 0;
        if (ehc_params->is_RtlRaise && ExceptionCode == EXCEPTION_BREAKPOINT) {
            // Restore Eip as we pretended there's a breakpoint.
            pContextRecord->Eip = (DWORD)pExceptionRecord->ExceptionAddress;
        }
        if (!ehc_params->is_RtlRaise) {
            // Assumed it get called again.
            pContextRecord->Esp -= 2;
        }
    }

    // Exception record test
    GEN_CHECK(ExceptionCode, ehc_params->ExceptionCode, "ExceptionCode");
    GEN_CHECK(pExceptionRecord->ExceptionCode, ehc_params->ExceptionCode, "pExceptionRecord->ExceptionCode");
    if (ehc_params->ExceptionHandlerReturn == EXCEPTION_CONTINUE_EXECUTION) {
        GEN_CHECK(pExceptionRecord->ExceptionFlags, 0, "pExceptionRecord->ExceptionFlags");
    } else {
        // NOTE: While there are EXCEPTION_UNWINDING and EXCEPTION_EXIT_UNWIND flags, it is best to let them handled internally.
        GEN_CHECK(pExceptionRecord->ExceptionFlags, EXCEPTION_NONCONTINUABLE, "pExceptionRecord->ExceptionFlags");
    }
    if (ehc_params->is_RtlRaiseStatus) {
        GEN_CHECK_RANGE((DWORD)pExceptionRecord->ExceptionAddress,
                        (DWORD)RtlRaiseStatus,
                        0x300/*assumed size since it is kernel function and reside in xbox hardware*/,
                        "ExceptionAddress");
    }
    else {
        GEN_CHECK_RANGE((DWORD)pExceptionRecord->ExceptionAddress,
                        (DWORD)func_exception,
                        0x300/*assumed size since can't find actual function size in C*/,
                        "ExceptionAddress");
    }
    // Right now, we only check two parameters.
    if (pExceptionRecord->NumberParameters && pExceptionRecord->NumberParameters >= 2) {
        GEN_CHECK(pExceptionRecord->ExceptionInformation[0], (DWORD)ehc_params->func_addr, "pExceptionRecord->ExceptionInformation[0]");
        GEN_CHECK(pExceptionRecord->ExceptionInformation[1], pExceptionRecord->NumberParameters, "pExceptionRecord->ExceptionInformation[1]");
    }
    for (unsigned i = 2; i < pExceptionRecord->NumberParameters; i++) {
        GEN_CHECK(pExceptionRecord->ExceptionInformation[i], 0/*Always 0*/, "pExceptionRecord->ExceptionInformation[i]");
    }

    GEN_CHECK(pExceptionRecord->ExceptionRecord, NULL, "pExceptionRecord->ExceptionRecord");

    // Context record test
    // When called RtlRaise(Exception|Status), they will subtract by one to context's Eip for assumed breakpoint.
    if (ehc_params->ExceptionHandlerReturn != EXCEPTION_CONTINUE_EXECUTION && ehc_params->is_RtlRaise && ExceptionCode == EXCEPTION_BREAKPOINT) {
        GEN_CHECK(pContextRecord->Eip, (DWORD)pExceptionRecord->ExceptionAddress - 1, "pContextRecord->Eip");
    }
    else {
        GEN_CHECK(pContextRecord->Eip, (DWORD)pExceptionRecord->ExceptionAddress, "pContextRecord->Eip");
    }

    if (ehc_params->is_RtlRaise) {
        GEN_CHECK(pContextRecord->ContextFlags,
                  (CONTEXT_X86 | CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_FLOATING_POINT | CONTEXT_EXTENDED_REGISTERS),
                  "pContextRecord->ContextFlags");
        // NOTE: Not sure if they are used to copy over.
        // SegCs = 0x00000008; SegSs = 0x00000010;
        // EFlags difference: (__Exception) 0x00000286; vs (__Status) 0x00000246;
    }
    else {
        GEN_CHECK(pContextRecord->ContextFlags,
                  (CONTEXT_X86 | CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS),
                  "pContextRecord->ContextFlags");
        // SegCs = 0x00030008; EFlags = 0x00000286; SegSs = 0x00030010;
    }

#if 1 // Debug purpose
    print("  context record debug: "/*FloatSave = */
          "Edi = 0x%08X; Esi = 0x%08X; Ebx = 0x%08X; Edx = 0x%08X; Ecx = 0x%08X; Eax = 0x%08X;"
          "Ebp = 0x%08X; Eip = 0x%08X; SegCs = 0x%08X; EFlags = 0x%08X; Esp = 0x%08X; SegSs = 0x%08X;",
          //pContextRecord->FloatSave,
          pContextRecord->Edi,
          pContextRecord->Esi,
          pContextRecord->Ebx,
          pContextRecord->Edx,
          pContextRecord->Ecx,
          pContextRecord->Eax,
          pContextRecord->Ebp,
          pContextRecord->Eip,
          pContextRecord->SegCs,
          pContextRecord->EFlags,
          pContextRecord->Esp,
          pContextRecord->SegSs);
#endif

    *ehc_params->ptests_passed &= test_passed;
    return ehc_params->ExceptionHandlerReturn;
}
