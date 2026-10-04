#ifndef HW_BITS_H
#define HW_BITS_H

#include <stdint.h>
#include <stdio.h>

/**
 * Prints a number (specified by width) of the lowest bits of a given integer.
 * Bits are output from most significant to least significant, and they are
 * grouped into nibbles. If width is out-of-bounds, an error is raised and the
 * function returns.
 * @param x The value to print the binary equivalent of.
 * @param width The number of bits to print. Values may range from 1-32.
 */
void print_binary(uint32_t x, int width);

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
uint32_t get_field(uint32_t word, int pos, int width);

/**
 * Returns the specified word with value inserted into the selected position.
 * Inserts the lowest width bits of value into word, then returns that new word
 * with all other bits unchanged. Any out-of-bounds values for pos or width will
 * result in an error, and the function will return word unmodified. If neither
 * pos nor width are invalid, but pos + width > 32, an error is raised and the
 * function returns word unmodified.
 * @param word The word to modify.
 * @param pos Position of the rightmost bit position of the modified portion of
 * word. Values may range from 0-31.
 * @param width Number of bits to read from value. Values may range from 1-32.
 * @param value The value to be inserted into word.
 * @return The modified word.
 */
uint32_t set_field(uint32_t word, int pos, int width, uint32_t value);

/**
 * Returns a portion of value interpreted as a two's complement number (signed).
 * Reads the lowest width bits of value, interprets it as a two's complement
 * number, performs the appropriate sign extension, and returns the value as an
 * int32_t. If width is out-of-bounds, raise an error and return 0.
 * @param value The value containing the two's complement number.
 * @param width Number of bits to read from value. Values may range from 1-32.
 */
int32_t sign_extend(uint32_t value, int width);

#endif