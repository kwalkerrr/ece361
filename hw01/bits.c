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

/**
 * Returns the specified consecutive bits of a word.
 * Returns bits pos to pos + width - 1 of word shifted down to bit 0. Any
 * out-of-bounds values for pos or width will result in an error, and the
 * function will return 0. If neither pos nor width are invalid, but pos + width
 * > 32, an error is raised and the function returns 0.
 * @param word The word to read.
 * @param pos Position of the rightmost bit to read. Values may range from 0-31.
 * @param width The number of bits to return. Values may range from 1-32.
 * @return The specified bits of the word.
 */
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