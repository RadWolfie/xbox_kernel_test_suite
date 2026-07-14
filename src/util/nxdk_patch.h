#pragma once

// Place missing nxdk implementations here to make them easy to track.
// TODO: Upstream this file's contents to the nxdk repo in a future pull request.
// Include this header in your test files for the time being.
// This file cannot be removed until all of the tests are fully implemented.

#ifndef STATUS_SUSPEND_COUNT_EXCEEDED
// TODO: Add below into nxdk's xboxkrnl/ntstatus.h file
#define STATUS_SUSPEND_COUNT_EXCEEDED 0xC000004A
#else
#warning "STATUS_SUSPEND_COUNT_EXCEEDED has been upstreamed; patch can be safely removed."
#endif
