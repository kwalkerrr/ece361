#include "bits.h"

void print_binary(uint32_t x, int width) {
    if (width < 1 || width > 32) {
        fprintf(stderr, "print_binary: width %d is out of range (1-32)\n",
            width);
        return;
    }

    for (int i = width - 1; i >= 0; i--) {
        putchar(((x >> i) & 1u) ? '1' : '0');
        if ((i % 4) == 0 && i > 0) {
            putchar(' ');
        }
    }
    putchar('\n');
}

uint32_t get_field(uint32_t word, int pos, int width) {
    if (pos < 0 || pos > 31) {
        fprintf(stderr, "get_field: pos %d is out of range (0-31)\n", pos);
        return 0;
    }
    if (width < 1 || width > 32) {
        fprintf(stderr, "get_field: width %d is out of range (1-32)\n",
            width);
        return 0;
    }
    if ((pos + width) > 32) {
        fprintf(stderr, "get_field: pos %d and width %d sum to > 32\n",
        pos, width);
        return 0;
    }

    uint32_t mask = (width == 32) ? UINT32_MAX : (UINT32_C(1) << width) - 1;
    return (word >> pos) & mask;
}

uint32_t set_field(uint32_t word, int pos, int width, uint32_t value) {
    if (pos < 0 || pos > 31) {
        fprintf(stderr, "set_field: pos %d is out of range (0-31)\n", pos);
        return word;
    }
    if (width < 1 || width > 32) {
        fprintf(stderr, "set_field: width %d is out of range (1-32)\n",
            width);
        return word;
    }
    if ((pos + width) > 32) {
        fprintf(stderr, "set_field: pos %d and width %d sum to > 32\n",
        pos, width);
        return word;
    }

    // set up value to be ORed with word
    uint32_t mask = (width == 32) ? UINT32_MAX : (UINT32_C(1) << width) -1;
    value &= mask;
    value <<= pos;

    // clear bits pos to pos + width - 1 in word
    mask <<= pos;
    mask = ~mask;
    word &= mask;

    // OR value with word
    word |= value;
    
    return word;
}

int32_t sign_extend(uint32_t value, int width) {
    if (width < 1 || width > 32) {
        fprintf(stderr, "sign_extend: width %d is out of range (1-32)\n",
            width);
        return 0;
    }

    if (width == 32) {
        return (int32_t)value; // cast value as int32_t
    }

    uint32_t mask = (UINT32_C(1) << width) - 1;
    value &= mask;
    if ((value >> (width - 1)) & 1u) { // check sign bit for selection
        value |= ~mask;
    }
    return (int32_t)value;
}