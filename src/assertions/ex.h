#pragma once

#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "util/output.h" // for print
#include "defines.h" // for GEN_CHECK

BOOL assert_ERWLOCK_equals(
    PERWLOCK,
    LONG,
    ULONG,
    ULONG,
    ULONG,
    const char*
);

static DWORD assert_exception_code_list[] = {
    EXCEPTION_ACCESS_VIOLATION,
    EXCEPTION_DATATYPE_MISALIGNMENT,
    EXCEPTION_BREAKPOINT,
    EXCEPTION_SINGLE_STEP,
    EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
    EXCEPTION_FLT_DENORMAL_OPERAND,
    EXCEPTION_FLT_DIVIDE_BY_ZERO,
    EXCEPTION_FLT_INEXACT_RESULT,
    EXCEPTION_FLT_INVALID_OPERATION,
    EXCEPTION_FLT_OVERFLOW,
    EXCEPTION_FLT_STACK_CHECK,
    EXCEPTION_FLT_UNDERFLOW,
    EXCEPTION_INT_DIVIDE_BY_ZERO,
    EXCEPTION_INT_OVERFLOW,
    EXCEPTION_PRIV_INSTRUCTION,
    EXCEPTION_IN_PAGE_ERROR,
    EXCEPTION_ILLEGAL_INSTRUCTION,
    EXCEPTION_NONCONTINUABLE_EXCEPTION,
    EXCEPTION_STACK_OVERFLOW,
    EXCEPTION_INVALID_DISPOSITION,
    EXCEPTION_GUARD_PAGE,
    EXCEPTION_INVALID_HANDLE,
};

typedef void (NTAPI *callback_func_raise)(void*);

typedef struct _ExceptionHandlerCatcherParams {
    const char* func_name;
    PVOID func_addr;
    BOOL is_RtlRaise;
    BOOL is_RtlRaiseStatus;
    BOOL* ptests_passed;
    DWORD ExceptionCode;
    int ExceptionHandlerReturn;
    callback_func_raise callback_func;
    void* callback_param;
} ExceptionHandlerCatcherParams;

int assert_ExceptionHandlerCatcher(ExceptionHandlerCatcherParams* ehc_params,
                                   void* func_exception,
                                   DWORD ExceptionCode,
                                   PEXCEPTION_POINTERS ExceptionInformation);

#define DEBUG_EXCEPTION_STEP // Debugging Cxbx-Reloaded through log file for faster analysis.
static void assert_ExceptionGenCheck(DWORD* except_steps, DWORD step, BOOL* ptests_passed) {
    BOOL test_passed = 1;
    const char* except_steps_str = "except_steps";
    GEN_CHECK(*except_steps, step, except_steps_str);
#ifdef DEBUG_EXCEPTION_STEP
    // Verbose debug if any steps missing.
    // NOTE: No steps were missing after thorough checks.
    print("  DEBUG: step = %u", step);
#endif
    *except_steps = step + 1;
    *ptests_passed &= test_passed;
}

//#define TEST_EXCEPTION_CONTINUE_EXECUTION // Disabled as NXDK doesn't work as intended
static void assert_ExceptionTryExceptFinally(ExceptionHandlerCatcherParams* ehc_params) {
    BOOL test_passed = 1;
    DWORD except_steps = 0;

#ifdef DEBUG_EXCEPTION_STEP
    // Verbose debug if any errors occur.
    print("  DEBUG: ExceptionCode = %08X test start", ehc_params->ExceptionCode);
#endif

    __try {

        // --- Test EXCEPTION_EXECUTE_HANDLER begin ----
        assert_ExceptionGenCheck(&except_steps, 0, &test_passed);
        __try {
            assert_ExceptionGenCheck(&except_steps, 1, &test_passed);
            __try {
                assert_ExceptionGenCheck(&except_steps, 2, &test_passed);
                __try {
                    assert_ExceptionGenCheck(&except_steps, 3, &test_passed);
                    ehc_params->ExceptionHandlerReturn = EXCEPTION_CONTINUE_SEARCH;
                    ehc_params->callback_func(ehc_params->callback_param);
                    // Should not be reachable
                    test_passed = 0;
                    print("  ERROR: Continued execution after call kernel function (1)");
                }
                __except(assert_ExceptionGenCheck(&except_steps, 4, &test_passed),
                         assert_ExceptionHandlerCatcher(ehc_params,
                                                        assert_ExceptionTryExceptFinally,
                                                        GetExceptionCode(),
                                                        GetExceptionInformation())
                         ) {
                    // Should not be reachable
                    test_passed = 0;
                    print("  ERROR: Should skip exception handler (2)");
                }
            }
            __finally {
                assert_ExceptionGenCheck(&except_steps, 6, &test_passed);
            }
        }
        __except(assert_ExceptionGenCheck(&except_steps, 5, &test_passed),
                 // Update for next exception to execute handler.
                 ehc_params->ExceptionHandlerReturn = EXCEPTION_EXECUTE_HANDLER,
                 assert_ExceptionHandlerCatcher(ehc_params,
                                                assert_ExceptionTryExceptFinally,
                                                GetExceptionCode(),
                                                GetExceptionInformation())
                 ) {
            assert_ExceptionGenCheck(&except_steps, 7, &test_passed);
        }
        // --- Test EXCEPTION_EXECUTE_HANDLER end ----

        assert_ExceptionGenCheck(&except_steps, 8, &test_passed);

#ifdef TEST_EXCEPTION_CONTINUE_EXECUTION
        // --- Test EXCEPTION_CONTINUE_EXECUTION begin ----
        __try {
            assert_ExceptionGenCheck(&except_steps, 9, &test_passed);
            __try {
                assert_ExceptionGenCheck(&except_steps, 10, &test_passed);
                __try {
                    assert_ExceptionGenCheck(&except_steps, 11, &test_passed);
                    ehc_params->ExceptionHandlerReturn = EXCEPTION_CONTINUE_SEARCH;
                    ehc_params->callback_func(ehc_params->callback_param);

                    // For continue execution, this should be the last step to process
                    assert_ExceptionGenCheck(&except_steps, 14, &test_passed);
                }
                __except(assert_ExceptionGenCheck(&except_steps, 12, &test_passed),
                         assert_ExceptionHandlerCatcher(ehc_params,
                                                        assert_ExceptionTryExceptFinally,
                                                        GetExceptionCode(),
                                                        GetExceptionInformation())
                         ) {
                    // Should not be reachable
                    test_passed = 0;
                    print("  ERROR: Should skip exception handler (3)");
                }
            }
            __finally {
                assert_ExceptionGenCheck(&except_steps, 15, &test_passed);
            }
        }
        // This triggered maybe unwind local variables back to step 12.
        // And update ExceptionCode to EXCEPTION_NONCONTINUABLE_EXCEPTION.
        // which also trigger finally first, then here repeatly.
        // TODO: Find out how to move forward in catcher function. And maybe preserve the values.
        __except(assert_ExceptionGenCheck(&except_steps, 13, &test_passed),
                 // Update for next exception to resume execute.
                 ehc_params->ExceptionHandlerReturn = EXCEPTION_CONTINUE_EXECUTION,
                 assert_ExceptionHandlerCatcher(ehc_params,
                                                assert_ExceptionTryExceptFinally,
                                                GetExceptionCode(),
                                                GetExceptionInformation())
                 ) {
             // Should not be reachable
            test_passed = 0;
            print("  ERROR: Should skip exception handler (4)");
        }
        // --- Test EXCEPTION_CONTINUE_EXECUTION end ----

        // Make sure we are still processing after the handled catchs above.
        assert_ExceptionGenCheck(&except_steps, 16, &test_passed);
#endif
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        // Should not be reachable
        test_passed = 0;
        print("  ERROR: Should skip exception handler (5)");
    }

#ifdef TEST_EXCEPTION_CONTINUE_EXECUTION
    assert_ExceptionGenCheck(&except_steps, 17, &test_passed);
#else
    assert_ExceptionGenCheck(&except_steps, 9, &test_passed);
#endif

    if (!test_passed) {
        print("  ERROR: ExceptionCode = %08X test failed, see above expected error(s)", ehc_params->ExceptionCode);
    }

#ifdef DEBUG_EXCEPTION_STEP
    print("  DEBUG: ExceptionCode = %08X test done", ehc_params->ExceptionCode);
#endif

    // Finally, set failure if any occur.
    *ehc_params->ptests_passed &= test_passed;
}
