#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"

TEST_FUNC(XcDESKeyParity)
{
	TEST_BEGIN();

	UCHAR key_des[8] = { 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17 };

	XcDESKeyParity(key_des, sizeof(key_des));
	if(memcmp(key_des, "\x10\x10\x13\x13\x15\x15\x16\x16", 8) == 0){
	    test_passed &= 1;
	}
	else {
	    test_passed &= 0;
	}

	TEST_END();
}
