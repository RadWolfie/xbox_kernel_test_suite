#include "exception.h"
#include "util/output.h"

BOOL GetExceptionStatusEx(NTSTATUS exception_code, NTSTATUS *status, BOOL ret) {
    *status = exception_code;
    return ret;
}

VOID GetExceptionInformationRecordsEx(PEXCEPTION_POINTERS records_pointer, PEXCEPTION_RECORD exception, PCONTEXT context) {
    if (exception) {
        if (records_pointer->ExceptionRecord) {
            *exception = *(records_pointer->ExceptionRecord);
            // We don't need to copy nested records, as doing so would require allocating memory
            // and a secondary cleanup function to free them
            exception->ExceptionRecord = NULL;
        }
        else {
            *exception = (EXCEPTION_RECORD){ 0 };
        }
    }
    if (context) {
        if (records_pointer->ContextRecord) {
            *context = *(records_pointer->ContextRecord);
        }
        else {
            *context = (CONTEXT){ 0 };
        }
    }
}

VOID DebugExceptionInformationRecordsEx(signed line, PEXCEPTION_RECORD exception, PCONTEXT context) {
    if (exception) {
        print("  DEBUG(%d): exception.%s = 0x%08X", line, "ExceptionCode", exception->ExceptionCode);
        print("  DEBUG(%d): exception.%s = 0x%08X", line, "ExceptionFlags", exception->ExceptionFlags);
        print("  DEBUG(%d): exception.%s = 0x%08X", line, "ExceptionRecord", exception->ExceptionRecord);
        print("  DEBUG(%d): exception.%s = 0x%08X", line, "ExceptionAddress", exception->ExceptionAddress);

        print("  DEBUG(%d): exception.%s = 0x%08X", line, "NumberParameters", exception->NumberParameters);
        for (unsigned i = 0; i < exception->NumberParameters; i++) {
            print("  DEBUG(%d): exception.%s[%u] = 0x%08X", line, "ExceptionInformation", i, exception->ExceptionInformation[i]);
        }
    }
    if (context) {
        // Context Flags
        print("  DEBUG(%d): context.%s = 0x%08X", line, "ContextFlags", context->ContextFlags);

        // General Purpose Registers
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EAX", context->Eax);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EBX", context->Ebx);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "ECX", context->Ecx);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EDX", context->Edx);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "ESI", context->Esi);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EDI", context->Edi);

        // Frame and Stack Pointers
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EBP", context->Ebp);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "ESP", context->Esp);

        // Execution Control Registers
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EIP", context->Eip);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "EFlags", context->EFlags);

        // Segment Registers
        print("  DEBUG(%d): context.%s = 0x%08X", line, "SegCs", context->SegCs);
        print("  DEBUG(%d): context.%s = 0x%08X", line, "SegSs", context->SegSs);
    }
}
