#pragma once

#include <excpt.h>

BOOL GetExceptionStatusEx(NTSTATUS exception_code, NTSTATUS *status, BOOL ret);

#define GetExceptionStatus(status, ret) GetExceptionStatusEx(GetExceptionCode(), status, ret)

VOID GetExceptionInformationRecordsEx(PEXCEPTION_POINTERS records_pointer, PEXCEPTION_RECORD exception, PCONTEXT context);

#define GetExceptionInformationRecords(exception, context) GetExceptionInformationRecordsEx(GetExceptionInformation(), exception, context)

VOID DebugExceptionInformationRecordsEx(signed line, PEXCEPTION_RECORD exception, PCONTEXT context);

#define DebugExceptionInformationRecords(exception, context) DebugExceptionInformationRecordsEx(__LINE__, exception, context)
