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