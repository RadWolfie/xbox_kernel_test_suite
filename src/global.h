#pragma once

#include "util/vector.h"

#define NV2A_MMIO_BASE 0xFD000000

extern const char* failed_text;
extern const char* passed_text;

extern unsigned int seed;
extern vector tests_to_run;
