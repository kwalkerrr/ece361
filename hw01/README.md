# hw01: Bit Manipulation and Thermostat Status Word

A small C11 library for reading, writing, and printing bit fields
(`bits.h`/`bits.c`), and for unpacking a 16-bit thermostat status word into a
struct (`status.h`/`status.c`).

## Building and Testing

```sh
make         # compile every .c file to a .o file and build the test programs
make test    # build, then run tests/test_bits and tests/test_status
make clean   # remove all .o files and test programs
```

Each test program prints `[PASS]` or `[FAIL]` for every check and exits with a
non-zero status if any check fails.

## Error Handling

No function in this library aborts the program. When a function receives an
invalid argument, it:

1. prints an error message to `stderr`, prefixed with the function's name
   (for example, `get_field: pos 32 is out of range (0-31)`), and
2. returns a safe fallback value, which is listed under each function below.

Because the fallback values can also be valid results (for example, `0`), the
`stderr` message is the only way to tell that an error occurred.

## bits.h

### `print_binary`

```c
void print_binary(uint32_t x, int width);
```

Prints the lowest `width` bits of `x` in binary, from most significant to
least significant, grouped into nibbles (groups of 4 bits).

| Parameter | Description                                    |
|-----------|------------------------------------------------|
| `x`       | The value to print in binary.                  |
| `width`   | The number of bits to print. Valid range: 1-32. |

**Example:** `print_binary(0xA5, 8)` prints `1010 0101`.

**Errors:** If `width` is out of range, an error is printed and nothing else
is printed.

### `get_field`

```c
uint32_t get_field(uint32_t word, int pos, int width);
```

Returns bits `pos` through `pos + width - 1` of `word`, shifted down to bit 0.

| Parameter | Description                                                |
|-----------|------------------------------------------------------------|
| `word`    | The word to read.                                          |
| `pos`     | Position of the rightmost bit to read. Valid range: 0-31.  |
| `width`   | The number of bits to return. Valid range: 1-32.           |

**Returns:** The selected bits of `word`.

**Example:** `get_field(0x12345678, 4, 8)` returns `0x67`.

**Errors:** Returns `0` if:
- `pos` is out of range,
- `width` is out of range, or
- `pos + width > 32` (the field would extend past bit 31).

### `set_field`

```c
uint32_t set_field(uint32_t word, int pos, int width, uint32_t value);
```

Inserts the lowest `width` bits of `value` into `word` starting at bit `pos`,
and returns the new word. All other bits of `word` are unchanged. Bits of
`value` above `width` are ignored.

| Parameter | Description                                                          |
|-----------|----------------------------------------------------------------------|
| `word`    | The word to modify.                                                  |
| `pos`     | Position of the rightmost bit of the field to modify. Valid range: 0-31. |
| `width`   | The number of bits to read from `value`. Valid range: 1-32.          |
| `value`   | The value to insert into `word`.                                     |

**Returns:** The modified word.

**Example:** `set_field(0x12345678, 8, 8, 0xAB)` returns `0x1234AB78`.

**Errors:** Returns `word` unmodified if:
- `pos` is out of range,
- `width` is out of range, or
- `pos + width > 32` (the field would extend past bit 31).

### `sign_extend`

```c
int32_t sign_extend(uint32_t value, int width);
```

Reads the lowest `width` bits of `value`, interprets them as a two's
complement (signed) number, and returns it sign-extended to an `int32_t`.

| Parameter | Description                                                  |
|-----------|--------------------------------------------------------------|
| `value`   | The value containing the two's complement number.            |
| `width`   | The number of bits to read from `value`. Valid range: 1-32.  |

**Returns:** The signed `int32_t` form of the selected bits.

**Example:** `sign_extend(0xA, 4)` returns `-6`.

**Errors:** Returns `0` if `width` is out of range.

## status.h

### `status_t`

```c
typedef struct {
    bool heat;
    bool cool;
    bool fan;
    bool fault;
    uint8_t mode;
    bool reserved;
    int8_t setpoint;
} status_t;
```

Holds the unpacked fields of a thermostat status word. See `status_unpack`
for what each member means.

### `status_unpack`

```c
status_t status_unpack(uint16_t word);
```

Unpacks a 16-bit thermostat status word into a `status_t`.

| Parameter | Description                                 |
|-----------|---------------------------------------------|
| `word`    | The 16-bit status word from the thermostat. |

**Returns:** A `status_t` with one member for each field of the status word.

The status word is laid out as follows:

| Bit(s) | Member     | Description                                                    |
|--------|------------|----------------------------------------------------------------|
| 0      | `heat`     | Heater on                                                      |
| 1      | `cool`     | Compressor on                                                  |
| 2      | `fan`      | Fan on                                                         |
| 3      | `fault`    | Fault detected                                                 |
| 4-6    | `mode`     | 0 = OFF, 1 = HEAT, 2 = COOL, 3 = AUTO, 4 = FAN ONLY; 5-7 invalid |
| 7      | `reserved` | Must be 0                                                      |
| 8-15   | `setpoint` | Set point in degrees Celsius, signed (-128 to 127)             |

**Example:** `status_unpack(0x1631)` returns `heat = true`, `mode = 3` (AUTO),
`setpoint = 22`, and all other members `false`.

**Errors:** All other members are still unpacked normally when an error occurs.
- **Invalid mode (5-7):** an error is printed and `mode` is set to `0`.
  Note that `0` is also the valid value for OFF.
- **Reserved bit set:** an error is printed and `reserved` is set to `false`.
