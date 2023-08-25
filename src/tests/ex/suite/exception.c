#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "assertions/ex.h"
#include "util/output.h"

void test_ExRaiseException()
{
    const char* func_num = "0x001A";
    const char* func_name = "ExRaiseException";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    EXCEPTION_RECORD exception_record;
    memset(&exception_record, 0, sizeof(EXCEPTION_RECORD));

    exception_record.ExceptionFlags = EXCEPTION_NONCONTINUABLE; // TODO: Extend test for this?
    //exception_record.ExceptionRecord = NULL;
    //exception_record.ExceptionAddress; // is overrided internally
    exception_record.NumberParameters = 5; // used for ExceptionInformation (one line below)
    exception_record.ExceptionInformation[0] = (DWORD)test_ExRaiseException;
    exception_record.ExceptionInformation[1] = exception_record.NumberParameters;
    //exception_record.ExceptionInformation[2]; // Unused
    //exception_record.ExceptionInformation[3]; // Unused
    //exception_record.ExceptionInformation[4]; // Unused

    // Fill in the fields to save time passing down to assert_ExceptionHandlerCatcher function.
    ExceptionHandlerCatcherParams ehc_params;
    memset(&ehc_params, 0, sizeof(ExceptionHandlerCatcherParams));
    ehc_params.func_name = func_name;
    ehc_params.func_addr = test_ExRaiseException;
    ehc_params.is_RtlRaise = FALSE;
    ehc_params.is_RtlRaiseStatus = FALSE;
    ehc_params.ptests_passed = &tests_passed;
    ehc_params.callback_func = (callback_func_raise)ExRaiseException;
    ehc_params.callback_param = &exception_record; // We only need to set this once.

    // Test list of exception codes
    for (unsigned i = 0; i < (sizeof(assert_exception_code_list) / sizeof(DWORD)); i++) {
        exception_record.ExceptionCode = assert_exception_code_list[i];
        ehc_params.ExceptionCode = exception_record.ExceptionCode;
        assert_ExceptionTryExceptFinally(&ehc_params);
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_ExRaiseStatus()
{
    const char* func_num = "0x001B";
    const char* func_name = "ExRaiseStatus";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // Fill in the fields to save time passing down to assert_ExceptionHandlerCatcher function.
    ExceptionHandlerCatcherParams ehc_params;
    memset(&ehc_params, 0, sizeof(ExceptionHandlerCatcherParams));
    ehc_params.func_name = func_name;
    ehc_params.func_addr = test_ExRaiseStatus;
    ehc_params.is_RtlRaise = FALSE;
    ehc_params.is_RtlRaiseStatus = FALSE;
    ehc_params.ptests_passed = &tests_passed;
    ehc_params.callback_func = (callback_func_raise)ExRaiseStatus;

    // Test list of exception codes
    for (unsigned i = 0; i < (sizeof(assert_exception_code_list) / sizeof(DWORD)); i++) {
        ehc_params.ExceptionCode = assert_exception_code_list[i];
        ehc_params.callback_param = (void*)ehc_params.ExceptionCode;
        assert_ExceptionTryExceptFinally(&ehc_params);
    }

    print_test_footer(func_num, func_name, tests_passed);
}
