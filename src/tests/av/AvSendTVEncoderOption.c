#include <xboxkrnl/xboxkrnl.h>

#include "global.h" // for NV2A_MMIO_BASE var
#include "util/output.h"

void test_AvSendTVEncoderOption(int func_num, const char* func_name)
{
    // FIXME: there are other functions such as AV_OPTION_QUERY_MODE, AV_QUERY_ENCODER_TYPE, AV_OPTION_WIDESCREEN etc
    // FIXME: this test is broken. I get inconsistent value from my real xbox
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    unsigned long res = 0;
    AvSendTVEncoderOption((void *)NV2A_MMIO_BASE, 6, 0, &res);
    print("AvSendTVEncoderOption: %lu (0=AV_PACK_NONE 1=AV_PACK_STANDARD 2=AV_PACK_RFU 3=AV_PACK_SCART 4=AV_PACK_HDTV 5=AV_PACK_VGA 6=AV_PACK_SVIDEO)", res);

    print_test_footer(func_num, func_name, tests_passed);
}
