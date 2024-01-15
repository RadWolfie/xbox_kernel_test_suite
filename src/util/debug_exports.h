#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>
#include <winnt.h>

#include "util/output.h"
#include "assertions/defines.h"

static void validate_pooltag(ULONG PoolTag, UCHAR* cPoolTag)
{
    cPoolTag[0] = ((PoolTag >> 24) & 0xFF);
    cPoolTag[1] = ((PoolTag >> 16) & 0xFF);
    cPoolTag[2] = ((PoolTag >> 8) & 0xFF);
    cPoolTag[3] = (PoolTag & 0xFF);
    for (unsigned i = 0; i < 4; i++) {
        if (!(cPoolTag[i] >= 'a' && cPoolTag[i] <= 'z') && !(cPoolTag[i] >= 'A' && cPoolTag[i] <= 'Z')) {
            cPoolTag[i] = '?';
        }
    }
}

static void validate_export_ex(DWORD data, const char* name)
{
    PDWORD export_kt_table = (PDWORD)((PXBE_FILE_HEADER)XBE_DEFAULT_BASE)->PointerToKernelThunkTable;
    if (data) {
        unsigned i = 0;
        do {
            if (export_kt_table[i] == data) {
                break;
            }
            i++;
        }
        while (i < 378);

        if (i < 378) {
            print("  INFO: %s = KernelThunkTable[%u]", name, i);
            return;
        }
        print("  INFO: %s = %x (UNKNOWN)", name, data);
        return;
    }

    print("  INFO: %s = NULL", name);
}
#define validate_export(_struct_m) validate_export_ex((DWORD)_struct_m, #_struct_m)

static void analyze_object_type(POBJECT_TYPE object_type)
{
    validate_export(object_type->AllocateProcedure);
    validate_export(object_type->FreeProcedure);
    validate_export(object_type->CloseProcedure);
    validate_export(object_type->DeleteProcedure);
    validate_export(object_type->ParseProcedure);
    //object_type->DefaultObject; // Always private I believe, check for non-null I guess...
    // there's IdexChannelObject export... might need to check that with Io...
    UCHAR cPoolTag[4];
    validate_pooltag(object_type->PoolTag, cPoolTag);
    print("  object_type->PoolTag = %c%c%c%c", cPoolTag[0], cPoolTag[1], cPoolTag[2], cPoolTag[3]);
}
