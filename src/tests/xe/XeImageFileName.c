#include <xboxkrnl/xboxkrnl.h>
#include <string.h>
#include <limits.h>

#include "util/output.h"
#include "assertions/defines.h"
#include "util/misc.h"

TEST_FUNC(XeImageFileName)
{
    TEST_BEGIN();

    // NOTE: This test does not verify valid characters

    // Test #1: Verify the filename is set before continuing with the test
    PANSI_STRING pName = (PANSI_STRING)XeImageFileName;
    GEN_CHECK(pName != NULL, TRUE, "filename_is_set");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #2: Check if it contains the minimum length necessary
    // Starting path must begin with "\Device\", minimum of 8
    // Minimum layout adds: 1-char device node + 1-char backslash separator + 1-char filename
    const USHORT minimum_length = 11;
    GEN_CHECK_RANGE(pName->Length, minimum_length, USHRT_MAX - minimum_length, "length");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #3: Check that maximum length is greater than or equal to the length, up to USHRT_MAX
    GEN_CHECK_RANGE(pName->MaximumLength, pName->Length, USHRT_MAX - pName->Length, "maximum_length");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #4: Buffer must not be NULL
    GEN_CHECK(pName->Buffer != NULL, TRUE, "buffer_is_set");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #5: Validate string structure integrity
    // ANSI_STRING buffers are NOT guaranteed to be null-terminated.
    // The authoritative length is the Length member, not a null terminator.
    // Verify no embedded nulls exist within the declared length, which would
    // indicate corruption or a malformed string.
    BOOL no_embedded_null = (memchr(pName->Buffer, '\0', pName->Length) == NULL);
    GEN_CHECK(no_embedded_null, TRUE, "contain_no_embedded_null");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #6: The path should start with "\Device\"
    const CHAR expected_device_prefix[8] = "\\Device\\";
    GEN_CHECK_ARRAY(pName->Buffer, expected_device_prefix, ARRAY_SIZE(expected_device_prefix), "starts_with_device_prefix");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Variables for isolating tests #7 and #8
    const PCHAR sub_buffer = pName->Buffer + ARRAY_SIZE(expected_device_prefix);
    size_t sub_length = pName->Length - ARRAY_SIZE(expected_device_prefix);

    // Test #7: Find the next backslash isolating the device node segment
    const PCHAR sub_buffer_next_slash = memchr(sub_buffer, '\\', sub_length);
    GEN_CHECK(sub_buffer_next_slash != NULL, TRUE, "has_path_separator_after_device_node");
    if (TEST_IS_FAILED) {
        TEST_END();
    }

    // Test #8: Make sure the device node is valid
    size_t device_node_length = sub_buffer_next_slash - sub_buffer;
    GEN_CHECK(device_node_length > 0, TRUE, "valid_device_node_length");

    // Test #9: Check for at least one character after the last backslash
    unsigned filename_length = 0;
    for (unsigned i = pName->Length; i > 0; i--) {
        if (pName->Buffer[i - 1] == '\\') {
            break;
        }
        filename_length++;
    }
    GEN_CHECK_RANGE(filename_length, 1, pName->Length - 1, "filename_length");

    TEST_END();
}
