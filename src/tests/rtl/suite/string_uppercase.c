#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "global.h" // for seed var
#include "util/output.h"
#include "assertions/defines.h"

void test_RtlUpcaseUnicodeChar()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeString()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpcaseUnicodeToMultiByteN()
{
    /* FIXME: This is a stub! implement this function! */
}

void test_RtlUpperChar()
{
    const char* func_num = "0x013C";
    const char* func_name = "RtlUpperChar";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    // These results are taken from running inputs 0 through 255 on a NTSC Xbox
    CHAR expected_outputs[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xF7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x3F,
    };
    CHAR result;

    for(uint16_t i = 0; i < 0x100; i++) {
        result = RtlUpperChar((CHAR)i);
        if(result != expected_outputs[i]) {
            tests_passed = 0;
            print("  Test FAILED. Input = '%c'(0x%x), result = '%c'(0x%x), expected ='%c'(0x%x)",
                 (CHAR)i, i, result, (uint8_t)result, expected_outputs[i], (uint8_t)expected_outputs[i]);
        }
    }

    print_test_footer(func_num, func_name, tests_passed);
}

void test_RtlUpperString()
{
    const char* func_num = "0x013D";
    const char* func_name = "RtlUpperString";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    char rnd_letter;
    char rnd_letters[101];

    srand(seed);
    for(int k=0; k<100; k++){
        rnd_letter = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
        rnd_letters[k] = rnd_letter;
    }
    rnd_letters[100] = '\0';

    ANSI_STRING src_str;
    ANSI_STRING res_str;
    char res_buf[256];

    /* Initialize res_buf so RtlInitAnsiString works correctly */
    for (int i=0; i<255; i++)
        res_buf[i] = '0';
    res_buf[255] = 0;

    RtlInitAnsiString(&res_str, res_buf);

    /* Empty String Test */
    RtlInitAnsiString(&src_str, "");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase String Test Failed");

    /* Lowercase String Test */
    RtlInitAnsiString(&src_str, "xbox");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase String Test Failed");

    /* Lowercase Single Character Test */
    RtlInitAnsiString(&src_str, "x");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "X", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Lowercase Single Character Test Failed");

    /* Uppercase Single Character Test */
    RtlInitAnsiString(&src_str, "X");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "X", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Uppercase Single Character Test Failed"); 

    /* 100 Lowercase Characters Test */
    RtlInitAnsiString(&src_str, rnd_letters);
    RtlUpperString(&res_str, &src_str);
    for(int k=0; k<100; k++){
        if(res_str.Buffer[k] != toupper(rnd_letters[k]))
            tests_passed = 0;
    }
    if(!tests_passed)
        print("RtlUpperString 100 Lowercase Characters Test Failed");

    /* Uppercase String Test */
    RtlInitAnsiString(&src_str, "XBOX");
    RtlUpperString(&res_str, &src_str);
    tests_passed &= strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0 ? 1 : 0;
    if(!tests_passed)
        print("RtlUpperString Uppercase String Test Failed");

    print_test_footer(func_num, func_name, tests_passed);
}
