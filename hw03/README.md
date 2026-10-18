# hw03: Pointer Functions

`ptr.c` implements four small functions on floats and float arrays that take
their arguments, and return their results, through pointers.

| Module              | Purpose                                                  |
|---------------------|----------------------------------------------------------|
| `ptr.h` / `ptr.c`   | `swap_float`, `minmax`, `find_above`, and `reverse`      |
| `tests/test_ptr.c`  | Unit tests for every function in `ptr.c`                 |

## Building and Testing

```sh
make         # same as make test
make test    # compile ptr.c and tests/test_ptr.c, link and run tests/test_ptr
make clean   # remove all .o files and tests/test_ptr
```

Everything is compiled with `gcc -std=c11 -Wall -Wextra -Wshadow
-fsanitize=address`. Each `.o` file is rebuilt when its `.c` file or `ptr.h`
changes.

**`tests/test_ptr`** unit-tests the functions in `ptr.c`:

- `swap_float` on two different values.
- `minmax` with the minimum and maximum at the start, in the middle, and at the
  end, with all values equal, with one value, with `n = 0`, and with a NULL
  array.
- `find_above` with two values above the threshold (only the first one's
  address must be returned), with none, with a value exactly equal to the
  threshold, with `n = 0`, and with a NULL array.
- `reverse` on odd and even lengths, two values, one value, `n = 0`, and a NULL
  array.

It prints `[PASS]` or `[FAIL]` for each test, then a summary, and exits with a
non-zero status if any test fails. The error cases also print their error
messages to `stderr`, so those lines are expected in the output.

## ptr.h

`minmax`, `find_above`, and `reverse` check their arguments. On a bad argument
they print a message to `stderr` starting with the function's name, such as
`minmax: n of 0 is < 1` or `reverse: a is a NULL pointer`, and return without
reading or changing the array. `n` is checked before the pointers.

### `swap_float`

```c
void swap_float(float *a, float *b);
```

Swaps the values of two floats.

| Parameter | Description              |
|-----------|--------------------------|
| `a`       | Address of first float.  |
| `b`       | Address of second float. |

**Errors:** If `a` or `b` are NULL, an error is printed to `stderr` and the
function returns.

### `minmax`

```c
int minmax(const float *a, int n, float *min, float *max);
```

Stores the smallest value of `a` in `*min` and the largest in `*max`.

| Parameter | Description                                   |
|-----------|-----------------------------------------------|
| `a`       | The array to read.                            |
| `n`       | The length of the array. Must be at least 1.  |
| `min`     | Address in which to store the minimum value.  |
| `max`     | Address in which to store the maximum value.  |

**Returns:** 0 on success, or -1 if `n` is less than 1 or any pointer is NULL.

**Errors:** On -1, an error is printed to `stderr` and `*min` and `*max` are
left unchanged.

### `find_above`

```c
const float *find_above(const float *a, int n, float threshold);
```

Finds the first value in `a` that is strictly greater than `threshold`. A value
equal to `threshold` does not count.

| Parameter   | Description                                            |
|-------------|--------------------------------------------------------|
| `a`         | The array to read.                                     |
| `n`         | The length of the array. Must be at least 1.           |
| `threshold` | The value that an element must be greater than.        |

**Returns:** A pointer to the first value of `a` above `threshold`, or NULL if
there is none. The pointer is `const` because it points into `a`, which the
function promises not to change.

**Errors:** If `n` is less than 1 or `a` is NULL, an error is printed to
`stderr` and NULL is returned.

### `reverse`

```c
void reverse(float *a, int n);
```

Reverses `a[0]` to `a[n - 1]` in place.

| Parameter | Description                                   |
|-----------|-----------------------------------------------|
| `a`       | The array to reverse.                         |
| `n`       | The length of the array. Must be at least 2.  |

**Errors:** If `n` is less than 2 or `a` is NULL, an error is printed to
`stderr` and the array is left unchanged.
