#include "../bits.h"
#include <stdio.h>

static int failures = 0;

/* Compares an unsigned result against its expected value and reports it. */
static void check_u32(const char *desc, uint32_t expected, uint32_t actual) {
    int pass = (expected == actual);
    printf("[%s] %-40s expected 0x%08X, actual 0x%08X\n",
        pass ? "PASS" : "FAIL", desc, (unsigned)expected, (unsigned)actual);
    if (!pass) {
        failures++;
    }
}

/* Compares a signed result against its expected value and reports it. */
static void check_i32(const char *desc, int32_t expected, int32_t actual) {
    int pass = (expected == actual);
    printf("[%s] %-40s expected %11ld, actual %11ld\n",
        pass ? "PASS" : "FAIL", desc, (long)expected, (long)actual);
    if (!pass) {
        failures++;
    }
}

/* Prints the expected output, then calls print_binary to show the actual. */
static void show_binary(const char *desc, uint32_t x, int width,
    const char *expected) {
    printf("%s\n  expected: %s\n  actual:   ", desc, expected);
    print_binary(x, width);
}

/* print_binary returns nothing, so its output is checked by eye. */
static void test_print_binary(void) {
    printf("\n=== print_binary (compare by eye) ===\n");
    show_binary("width 32", 0xDEADBEEF, 32,
        "1101 1110 1010 1101 1011 1110 1110 1111");
    show_binary("width 1, bit set", 0x1, 1, "1");
    show_binary("width 1, value wider than width", 0x2, 1, "0");
    show_binary("width 4, value wider than width", 0xFF, 4, "1111");
    show_binary("width 8", 0xA5, 8, "1010 0101");
    show_binary("width 6 (partial nibble)", 0x5, 6, "00 0101");
    show_binary("width 0 (error)", 0x1, 0, "<error message, no output>");
    show_binary("width 33 (error)", 0x1, 33, "<error message, no output>");
}

static void test_get_field(void) {
    printf("\n=== get_field ===\n");
    check_u32("pos 0, width 32", 0xDEADBEEF, get_field(0xDEADBEEF, 0, 32));
    check_u32("pos 0, width 1", 0x1, get_field(0xDEADBEEF, 0, 1));
    check_u32("pos 31, width 1", 0x1, get_field(0xDEADBEEF, 31, 1));
    check_u32("pos 4, width 8", 0x67, get_field(0x12345678, 4, 8));
    check_u32("pos 16, width 16", 0x1234, get_field(0x12345678, 16, 16));
    check_u32("pos 8, width 12", 0xDBE, get_field(0xDEADBEEF, 8, 12));

    // error cases return 0; 0xFFFFFFFF makes a wrong result obvious
    check_u32("pos -1 (error)", 0, get_field(0xFFFFFFFF, -1, 4));
    check_u32("pos 32 (error)", 0, get_field(0xFFFFFFFF, 32, 1));
    check_u32("width 0 (error)", 0, get_field(0xFFFFFFFF, 0, 0));
    check_u32("width 33 (error)", 0, get_field(0xFFFFFFFF, 0, 33));
    check_u32("pos 1 + width 32 > 32 (error)", 0,
        get_field(0xFFFFFFFF, 1, 32));
    check_u32("pos 31 + width 2 > 32 (error)", 0,
        get_field(0xFFFFFFFF, 31, 2));
}

static void test_set_field(void) {
    printf("\n=== set_field ===\n");
    check_u32("pos 0, width 32", 0xCAFEBABE,
        set_field(0x00000000, 0, 32, 0xCAFEBABE));
    check_u32("pos 0, width 1", 0xFFFFFFFE, set_field(0xFFFFFFFF, 0, 1, 0));
    check_u32("pos 31, width 1, set", 0x80000000,
        set_field(0x00000000, 31, 1, 1));
    check_u32("pos 31, width 1, clear", 0x7FFFFFFF,
        set_field(0xFFFFFFFF, 31, 1, 0));
    check_u32("value wider than width", 0x000000F0,
        set_field(0x00000000, 4, 4, 0xFF));
    check_u32("pos 8, width 8", 0x1234AB78, set_field(0x12345678, 8, 8, 0xAB));
    check_u32("pos 12, width 8, clear", 0xFFF00FFF,
        set_field(0xFFFFFFFF, 12, 8, 0x00));

    // error cases return word unmodified
    check_u32("pos -1 (error)", 0x12345678, set_field(0x12345678, -1, 4, 0));
    check_u32("pos 32 (error)", 0x12345678, set_field(0x12345678, 32, 1, 0));
    check_u32("width 0 (error)", 0x12345678, set_field(0x12345678, 0, 0, 0));
    check_u32("width 33 (error)", 0x12345678,
        set_field(0x12345678, 0, 33, 0));
    check_u32("pos 1 + width 32 > 32 (error)", 0x12345678,
        set_field(0x12345678, 1, 32, 0));
    check_u32("pos 31 + width 2 > 32 (error)", 0x12345678,
        set_field(0x12345678, 31, 2, 0));
}

static void test_sign_extend(void) {
    printf("\n=== sign_extend ===\n");
    check_i32("width 1, sign bit set", -1, sign_extend(0x1, 1));
    check_i32("width 1, sign bit clear", 0, sign_extend(0x0, 1));
    check_i32("width 32, most negative", INT32_MIN, sign_extend(0x80000000, 32));
    check_i32("width 32, most positive", INT32_MAX, sign_extend(0x7FFFFFFF, 32));
    check_i32("width 32, all ones", -1, sign_extend(0xFFFFFFFF, 32));
    check_i32("width 8, most negative", -128, sign_extend(0x80, 8));
    check_i32("width 8, value wider than width", 127, sign_extend(0xFF7F, 8));
    check_i32("width 4, value wider than width", -8, sign_extend(0x1F8, 4));
    check_i32("width 4, positive", 5, sign_extend(0x5, 4));
    check_i32("width 4, negative", -6, sign_extend(0xA, 4));
    check_i32("width 12, all ones", -1, sign_extend(0xFFF, 12));

    // error cases return 0; 0xFFFFFFFF makes a wrong result obvious
    check_i32("width 0 (error)", 0, sign_extend(0xFFFFFFFF, 0));
    check_i32("width 33 (error)", 0, sign_extend(0xFFFFFFFF, 33));
}

int main(void) {
    // unbuffered so error messages on stderr line up with their test
    setvbuf(stdout, NULL, _IONBF, 0);

    test_print_binary();
    test_get_field();
    test_set_field();
    test_sign_extend();

    printf("\n%d checked test(s) failed\n", failures);
    return failures == 0 ? 0 : 1;
}
